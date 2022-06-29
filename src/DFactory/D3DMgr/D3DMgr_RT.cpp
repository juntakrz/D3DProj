#include "D3DMgr.h"

bool D3DMgr::CreateRenderTarget(std::string name, int16_t width, int16_t height, bool isHDR) noexcept
{
	// default parameters / undefined = use screen sized resources
	(width < 0 || height < 0) ?
		width = m_VWidth, height = m_VHeight : 0;

	// initialize new buffer entry, return result 'false' if already exists
	if (!renderTargets.try_emplace(name, std::make_unique<RenderTarget>()).second)
	{
		return false;
	};

	//create texture to be used as a render target
	D3D11_TEXTURE2D_DESC texDesc{};
	isHDR ? 
		texDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT
		: texDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
	texDesc.Width = width;
	texDesc.Height = height;
	texDesc.MipLevels = 1;
	texDesc.ArraySize = 1;
	texDesc.SampleDesc.Count = 1;
	texDesc.Usage = D3D11_USAGE_DEFAULT;
	texDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;
	texDesc.CPUAccessFlags = 0;
	texDesc.MiscFlags = 0;

	Device()->CreateTexture2D(&texDesc, nullptr, &renderTargets.at(name)->pTex);

	D3D11_RENDER_TARGET_VIEW_DESC RTVDesc{};
	RTVDesc.Format = texDesc.Format;
	RTVDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
	RTVDesc.Texture2D.MipSlice = 0;

	Device()->CreateRenderTargetView(renderTargets.at(name)->pTex.Get(), &RTVDesc, &renderTargets.at(name)->pRTV);

	D3D11_SHADER_RESOURCE_VIEW_DESC SRVDesc{};
	SRVDesc.Format = texDesc.Format;
	SRVDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	SRVDesc.Texture2D.MipLevels = 1;
	SRVDesc.Texture2D.MostDetailedMip = 0;

	Device()->CreateShaderResourceView(renderTargets.at(name)->pTex.Get(), &SRVDesc, &renderTargets.at(name)->pSRV);

	// store target's name
	renderTargets.at(name)->name = name;

	// store resolution in newly created render target entry
	renderTargets.at(name)->width = width;
	renderTargets.at(name)->height = height;

	// prepare viewport description
	renderTargets.at(name)->vp.Width = width;
	renderTargets.at(name)->vp.Height = height;
	renderTargets.at(name)->vp.MinDepth = 0.0f;
	renderTargets.at(name)->vp.MaxDepth = 1.0f;
	renderTargets.at(name)->vp.TopLeftX = 0.0f;

	// return successful result
	return true;
}

bool D3DMgr::CreateDepthTarget(std::string name, std::string srcTarget, uint8_t SRVMode, uint8_t usage) noexcept
{
	// check if provided render target exists
	if (renderTargets.find(srcTarget) == renderTargets.end())
	{
#ifdef _DEBUG || _DFDEBUG
		std::string msg = "ERROR: D3DMgr: render target '" + srcTarget + "' not found, unable to create depth target.\n";
		OutputDebugStringA(msg.c_str());
#endif
		return false;
	}

	// check if the depth target with the same name already exists
	if(!depthTargets.try_emplace(name, std::make_unique<DSTarget>()).second)
	{
#ifdef _DEBUG || _DFDEBUG
		std::string msg = "WARNING: D3DMgr: depth target '" + name + "' already exists. Skipping creation.\n";
		OutputDebugStringA(msg.c_str());
#endif
		return false;
	}

	uint16_t width = renderTargets.at(srcTarget)->width;
	uint16_t height = renderTargets.at(srcTarget)->height;

	// create depth stencil texture
	D3D11_TEXTURE2D_DESC stencilDesc = {};
	stencilDesc.Usage = D3D11_USAGE_DEFAULT;
	stencilDesc.Width = width;
	stencilDesc.Height = height;
	stencilDesc.MipLevels = 1u;
	stencilDesc.ArraySize = 1u;
	stencilDesc.SampleDesc.Count = 1u;
	stencilDesc.SampleDesc.Quality = 0u;
	stencilDesc.Format = DF::GetDepthFormatTypeless(usage);
	stencilDesc.BindFlags = (SRVMode)
		? D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE : D3D11_BIND_DEPTH_STENCIL;

	D3D_THROW_INFO(m_pDevice->CreateTexture2D(&stencilDesc, nullptr, &depthTargets.at(name)->pDSTex));

	// create depth stencil view
	D3D11_DEPTH_STENCIL_VIEW_DESC dsvd = {};
	dsvd.Format = DF::GetDepthFormatTyped(usage);
	dsvd.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	dsvd.Texture2D.MipSlice = 0u;

	D3D_THROW_INFO(m_pDevice->CreateDepthStencilView(depthTargets.at(name)->pDSTex.Get(), &dsvd, &depthTargets.at(name)->pDSV));

	depthTargets.at(name)->name = name;

	depthTargets.at(name)->width = width;
	depthTargets.at(name)->height = height;

	if (SRVMode)	// if SRV mode is Depth or DepthStencil - create SRV for depth
	{
		D3D11_SHADER_RESOURCE_VIEW_DESC SRVDesc{};
		SRVDesc.Format = DF::GetDepthFormatColor(usage);
		SRVDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		SRVDesc.Texture2D.MipLevels = 1;
		SRVDesc.Texture2D.MostDetailedMip = 0;

		Device()->CreateShaderResourceView(depthTargets.at(name)->pDSTex.Get(), &SRVDesc, &depthTargets.at(name)->pDepthSRV);
	}
	if (SRVMode == DF::DS_SRVMode::DepthStencil)	// if SRV mode is DepthStencil - create SRV for stencil
	{
		D3D11_SHADER_RESOURCE_VIEW_DESC SRVDesc{};
		SRVDesc.Format = DXGI_FORMAT_X24_TYPELESS_G8_UINT;
		SRVDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		SRVDesc.Texture2D.MipLevels = 1;
		SRVDesc.Texture2D.MostDetailedMip = 0;

		Device()->CreateShaderResourceView(depthTargets.at(name)->pDSTex.Get(), &SRVDesc, &depthTargets.at(name)->pStencilSRV);
	}

	return true;
}

bool D3DMgr::CreateDepthTarget(std::string name, int16_t width, int16_t height, uint8_t SRVMode, uint8_t usage) noexcept
{
	// check if the depth target with the same name already exists
	if (!depthTargets.try_emplace(name, std::make_unique<DSTarget>()).second)
	{
#ifdef _DEBUG || _DFDEBUG
		std::string msg = "WARNING: D3DMgr: depth target '" + name + "' already exists. Skipping creation.\n";
		OutputDebugStringA(msg.c_str());
#endif
		return false;
	}

	// create depth stencil texture
	D3D11_TEXTURE2D_DESC stencilDesc = {};
	stencilDesc.Usage = D3D11_USAGE_DEFAULT;
	stencilDesc.Width = width;
	stencilDesc.Height = height;
	stencilDesc.MipLevels = 1u;
	stencilDesc.ArraySize = 1u;
	stencilDesc.SampleDesc.Count = 1u;
	stencilDesc.SampleDesc.Quality = 0u;
	stencilDesc.Format = DF::GetDepthFormatTypeless(usage);
	stencilDesc.BindFlags = (SRVMode)
		? D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE : D3D11_BIND_DEPTH_STENCIL;

	D3D_THROW_INFO(m_pDevice->CreateTexture2D(&stencilDesc, nullptr, &depthTargets.at(name)->pDSTex));

	// create depth stencil view
	D3D11_DEPTH_STENCIL_VIEW_DESC dsvd = {};
	dsvd.Format = DF::GetDepthFormatTyped(usage);
	dsvd.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	dsvd.Texture2D.MipSlice = 0u;

	D3D_THROW_INFO(m_pDevice->CreateDepthStencilView(depthTargets.at(name)->pDSTex.Get(), &dsvd, &depthTargets.at(name)->pDSV));

	depthTargets.at(name)->name = name;

	depthTargets.at(name)->width = width;
	depthTargets.at(name)->height = height;

	if (SRVMode)	// if SRV mode is Depth or DepthStencil - create SRV for depth
	{
		D3D11_SHADER_RESOURCE_VIEW_DESC SRVDesc{};
		SRVDesc.Format = DF::GetDepthFormatColor(usage);
		SRVDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		SRVDesc.Texture2D.MipLevels = 1;
		SRVDesc.Texture2D.MostDetailedMip = 0;

		Device()->CreateShaderResourceView(depthTargets.at(name)->pDSTex.Get(), &SRVDesc, &depthTargets.at(name)->pDepthSRV);
	}
	if (SRVMode == DF::DS_SRVMode::DepthStencil)	// if SRV mode is DepthStencil - create SRV for stencil
	{
		D3D11_SHADER_RESOURCE_VIEW_DESC SRVDesc{};
		SRVDesc.Format = DXGI_FORMAT_X24_TYPELESS_G8_UINT;
		SRVDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		SRVDesc.Texture2D.MipLevels = 1;
		SRVDesc.Texture2D.MostDetailedMip = 0;

		Device()->CreateShaderResourceView(depthTargets.at(name)->pDSTex.Get(), &SRVDesc, &depthTargets.at(name)->pStencilSRV);
	}
	/*
	// standalone depth buffer provides its own viewport data to RTBind
	depthTargets.back().vp.Width = width;
	depthTargets.back().vp.Height = height;
	depthTargets.back().vp.MinDepth = 0.0f;
	depthTargets.back().vp.MaxDepth = 1.0f;
	depthTargets.back().vp.TopLeftX = 0.0f;
	*/
	return true;
}

bool D3DMgr::CreateCompatibleTarget(std::string name, std::string srcTarget, bool isDepthBuffer, bool createView) noexcept
{
	D3D11_TEXTURE2D_DESC texDesc;
	D3D11_RENDER_TARGET_VIEW_DESC rtvDesc;
	D3D11_DEPTH_STENCIL_VIEW_DESC dsvDesc;
	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;

	switch (isDepthBuffer)
	{
	case true:
	{
		// check if provided source target exists
		if (depthTargets.find(srcTarget) == depthTargets.end())
		{
#ifdef _DEBUG || _DFDEBUG
			std::string msg = "ERROR: D3DMgr: depth target '" + srcTarget + "' not found, unable to create a copy.\n";
			OutputDebugStringA(msg.c_str());
#endif
			return false;
		}

		// check if the depth target with the same name already exists
		if (!depthTargets.try_emplace(name, std::make_unique<DSTarget>()).second)
		{
#ifdef _DEBUG || _DFDEBUG
			std::string msg = "WARNING: D3DMgr: depth target '" + name + "' already exists. Overwriting.\n";
			OutputDebugStringA(msg.c_str());
#endif
		}

		depthTargets.at(srcTarget)->pDSTex->GetDesc(&texDesc);
		depthTargets.at(srcTarget)->pDepthSRV->GetDesc(&srvDesc);
		(createView) ? depthTargets.at(srcTarget)->pDSV->GetDesc(&dsvDesc) : void();

		Device()->CreateTexture2D(&texDesc, nullptr, &depthTargets.at(name)->pDSTex);
		Device()->CreateShaderResourceView(depthTargets.at(name)->pDSTex.Get(), &srvDesc, &depthTargets.at(name)->pDepthSRV);
		(createView)
			? Device()->CreateDepthStencilView(depthTargets.at(name)->pDSTex.Get(), &dsvDesc, &depthTargets.at(name)->pDSV) : 0u;

		// if depth target has a stencil shader resource view - create a compatible SRV for it as well
		if (depthTargets.at(srcTarget)->pStencilSRV)
		{
			depthTargets.at(srcTarget)->pStencilSRV->GetDesc(&srvDesc);
			Device()->CreateShaderResourceView(depthTargets.at(name)->pDSTex.Get(), &srvDesc, &depthTargets.at(name)->pStencilSRV);
		}

		return true;
	}
	case false:
	{
		// check if provided source target exists
		if (renderTargets.find(srcTarget) == renderTargets.end())
		{
#ifdef _DEBUG || _DFDEBUG
			std::string msg = "ERROR: D3DMgr: render target '" + srcTarget + "' not found, unable to create a copy.\n";
			OutputDebugStringA(msg.c_str());
#endif
			return false;
		}

		// check if the depth target with the same name already exists
		if (!renderTargets.try_emplace(name, std::make_unique<RenderTarget>()).second)
		{
#ifdef _DEBUG || _DFDEBUG
			std::string msg = "WARNING: D3DMgr: depth target '" + name + "' already exists. Overwriting.\n";
			OutputDebugStringA(msg.c_str());
#endif
		}

		renderTargets.at(srcTarget)->pTex->GetDesc(&texDesc);
		renderTargets.at(srcTarget)->pSRV->GetDesc(&srvDesc);
		(createView) ? renderTargets.at(srcTarget)->pRTV->GetDesc(&rtvDesc) : void();

		renderTargets.at(name)->vp = renderTargets.at(srcTarget)->vp;

		Device()->CreateTexture2D(&texDesc, nullptr, &renderTargets.at(name)->pTex);
		Device()->CreateShaderResourceView(renderTargets.at(name)->pTex.Get(), &srvDesc, &renderTargets.at(name)->pSRV);
		(createView)
			? Device()->CreateRenderTargetView(renderTargets.at(name)->pTex.Get(), &rtvDesc, &renderTargets.at(name)->pRTV) : 0u;

		return true;
	}
	}

	return false;
}

void D3DMgr::RTInitBuffers(bool isHDR) noexcept
{
	// create render and depth buffers
	CreateRenderTarget("rtMain", m_VWidth, m_VHeight, isHDR);
	CreateDepthTarget("dsMain", "rtMain", DF::DS_SRVMode::Depth);

	// create aux render buffer for blur pass
	CreateRenderTarget("rtBlur", m_VWidth, m_VHeight, isHDR);
	CreateDepthTarget("dsBlur", "rtBlur", DF::DS_SRVMode::Depth);

	// create layer mixing render target and its copy
	CreateCompatibleTarget("rtMix", "rtMain", false, true);
	CreateCompatibleTarget("rtMixCopy", "rtMix", false, true);

	// create downsampled render buffer for blur post processing
	CreateRenderTarget("rtPPBlur", m_VWidth / 2, m_VHeight / 2, isHDR);
	CreateCompatibleTarget("rtPPBlurCopy", "rtPPBlur", false, true);

	// create downsampled render buffer for bloom post processing
	CreateRenderTarget("rtPPBloom", m_VWidth / 2, m_VHeight / 2, isHDR);
	CreateCompatibleTarget("rtPPStore2X", "rtPPBloom", false, true);

	// create depth buffer for shadows
	CreateDepthTarget("dsCSM", DF::CSM::bufferSize * DF::CSM::cascades, DF::CSM::bufferSize, true, DF::DS_Usage::DepthShadow);

	// buffer clone of 'dsMain' so it can be used as a texture
	CreateCompatibleTarget("dsMainCopy", "dsMain", true, false);
}

void D3DMgr::RTBind(const std::string& renderTarget, const std::string& depthTarget, const uint8_t& num)
{
	// no error prevention for speed, define targets with care
	D3D_THROW_INFO(m_pContext->OMSetRenderTargets(
		num,
		(renderTarget != "") ? renderTargets.at(renderTarget)->pRTV.GetAddressOf() : &nullRTV,
		(depthTarget != "") ? depthTargets.at(depthTarget)->pDSV.Get() : nullDSV)
	);

	if (renderTarget != "")
	{
		D3D_THROW_INFO(m_pContext->RSSetViewports(1u, &renderTargets.at(renderTarget)->vp));
	};

	m_RTId = renderTarget;
	m_DSId = depthTarget;
	m_RTNum = num;
}

void D3DMgr::RTCopyTarget(const std::string& srcTarget, const std::string& destTarget, bool isDepthBuffer) noexcept
{
	// no error prevention for speed, define targets with care
	switch (isDepthBuffer)
	{
	case true:
	{
		//unbind render target if bound
		(m_DSId == srcTarget || m_DSId == destTarget) ? Context()->OMSetRenderTargets(0u, nullptr, nullptr) : void();

		//copy shader resource / texture
		Context()->CopyResource(depthTargets.at(destTarget)->pDSTex.Get(), depthTargets.at(srcTarget)->pDSTex.Get());

		//rebind render target if it was unbound before
		(m_DSId == srcTarget || m_DSId == destTarget)
			? Context()->OMSetRenderTargets(
				m_RTNum,
				(m_RTId != "") ? renderTargets.at(m_RTId)->pRTV.GetAddressOf() : &nullRTV,
				(m_DSId != "") ? depthTargets.at(m_DSId)->pDSV.Get() : nullDSV
			) : void();

		return;
	}
	case false:
	{
		//unbind render target if bound
		(m_RTId == srcTarget || m_RTId == destTarget) ? Context()->OMSetRenderTargets(0u, nullptr, nullptr) : void();

		//copy shader resource / texture
		Context()->CopyResource(renderTargets.at(destTarget)->pTex.Get(), renderTargets.at(srcTarget)->pTex.Get());

		//rebind render target if it was unbound before
		(m_RTId == srcTarget || m_RTId == destTarget)
			? Context()->OMSetRenderTargets(
				m_RTNum,
				(m_RTId != "") ? renderTargets.at(m_RTId)->pRTV.GetAddressOf() : &nullRTV,
				(m_DSId != "") ? depthTargets.at(m_DSId)->pDSV.Get() : nullDSV
			) : void();

		return;
	}
	}
}

ID3D11RenderTargetView* D3DMgr::GetRTView(const std::string& name) noexcept
{
	return renderTargets.at(name)->pRTV.Get();
}

ID3D11DepthStencilView* D3DMgr::GetDSView(const std::string& name) noexcept
{
	return depthTargets.at(name)->pDSV.Get();
}

D3DMgr::RenderTarget* D3DMgr::GetRenderTarget(const std::string& name) noexcept
{
	return renderTargets.at(name).get();
}

D3DMgr::DSTarget* D3DMgr::GetDepthTarget(const std::string& name) noexcept
{
	return depthTargets.at(name).get();
}

void D3DMgr::RTSet(ID3D11RenderTargetView* pRTV, ID3D11DepthStencilView* pDSV, uint8_t num) noexcept
{
	Context()->OMSetRenderTargets(num, &pRTV, pDSV);
}

bool D3DMgr::RTRemove(const std::string& name) noexcept
{
	if (renderTargets.find(name) != renderTargets.end())
	{
		renderTargets.erase(name);
		return true;
	}

	if (depthTargets.find(name) != depthTargets.end())
	{
		depthTargets.erase(name);
		return true;
	}

#ifdef _DEBUG || _DFDEBUG
	std::string msg = "D3DMgr: target '" + name + "' not found. Unable to delete.\n";
	OutputDebugStringA(msg.c_str());
#endif
	return false;
}

bool D3DMgr::RTSetAsShaderResource(const char* id, const uint8_t& shaderType, const uint8_t& slot) noexcept
{
	if (renderTargets.find(id) != renderTargets.end())
	{
		switch (shaderType)
		{
		case(DF::ShaderType::GS):
		{
			(renderTargets.at(id)->pSRV) ? Context()->GSSetShaderResources(slot, 1u, renderTargets.at(id)->pSRV.GetAddressOf()) : void();
			return true;
		}
		case(DF::ShaderType::PS):
		{
			(renderTargets.at(id)->pSRV) ? Context()->PSSetShaderResources(slot, 1u, renderTargets.at(id)->pSRV.GetAddressOf()) : void();
			return true;
		}
		}
	}

	if (depthTargets.find(id) != depthTargets.end())
	{
		switch (shaderType)
		{
		case(DF::ShaderType::GS):
		{
			(depthTargets.at(id)->pDepthSRV) ? Context()->GSSetShaderResources(slot, 1u, depthTargets.at(id)->pDepthSRV.GetAddressOf()) : void();
			return true;
		}
		case(DF::ShaderType::PS):
		{
			(depthTargets.at(id)->pDepthSRV) ? Context()->PSSetShaderResources(slot, 1u, depthTargets.at(id)->pDepthSRV.GetAddressOf()) : void();

			// bind stencil view to a next slot if present
			(depthTargets.at(id)->pStencilSRV) ? Context()->PSSetShaderResources(slot + 1, 1u, depthTargets.at(id)->pStencilSRV.GetAddressOf()) : void();
			return true;
		}
		}
	}

	return false;
}

void D3DMgr::RTClearShaderResource(const uint8_t& shaderType, const uint8_t& slot) noexcept
{
	ID3D11ShaderResourceView* pNullSRV = nullptr;

	switch (shaderType)
	{
	case(DF::ShaderType::GS):
	{
		
		Context()->GSSetShaderResources(slot, 1u, &pNullSRV);
		return;
	}
	case(DF::ShaderType::PS):
	{
		Context()->PSSetShaderResources(slot, 1u, &pNullSRV);
		return;
	}
	}
}

void D3DMgr::CreateRenderSR(bool HDR) noexcept
{
	//create texture to be used as a copy of render buffer
	D3D11_TEXTURE2D_DESC texDesc{};
	HDR ?
		texDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT
		: texDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
	texDesc.Width = m_VWidth;
	texDesc.Height = m_VHeight;
	texDesc.MipLevels = 1;
	texDesc.ArraySize = 1;
	texDesc.SampleDesc.Count = 1;
	texDesc.Usage = D3D11_USAGE_DEFAULT;
	texDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;
	texDesc.CPUAccessFlags = 0;
	texDesc.MiscFlags = D3D11_RESOURCE_MISC_GENERATE_MIPS;

	Device()->CreateTexture2D(&texDesc, nullptr, &m_pBufCopy_Tex);

	D3D11_SHADER_RESOURCE_VIEW_DESC SRVDesc{};
	SRVDesc.Format = texDesc.Format;
	SRVDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	SRVDesc.Texture2D.MipLevels = 1;
	SRVDesc.Texture2D.MostDetailedMip = 0;

	Device()->CreateShaderResourceView(m_pBufCopy_Tex.Get(), &SRVDesc, &m_pBufCopy_SR);
}