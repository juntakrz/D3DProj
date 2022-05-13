#include "D3DMgr.h"

uint8_t D3DMgr::CreateRenderBuffer(int16_t width, int16_t height, bool isHDR) noexcept
{
	// default parameters / undefined = use screen sized resources
	(width < 0 || height < 0) ?
		width = m_VWidth, height = m_VHeight : 0;

	// initialize new buffer entry
	renderTargets.push_back(RenderTarget{});

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

	Device()->CreateTexture2D(&texDesc, nullptr, &renderTargets.back().pTex);

	D3D11_RENDER_TARGET_VIEW_DESC RTVDesc{};
	RTVDesc.Format = texDesc.Format;
	RTVDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
	RTVDesc.Texture2D.MipSlice = 0;

	Device()->CreateRenderTargetView(renderTargets.back().pTex.Get(), &RTVDesc, &renderTargets.back().pRTV);

	D3D11_SHADER_RESOURCE_VIEW_DESC SRVDesc{};
	SRVDesc.Format = texDesc.Format;
	SRVDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	SRVDesc.Texture2D.MipLevels = 1;
	SRVDesc.Texture2D.MostDetailedMip = 0;

	Device()->CreateShaderResourceView(renderTargets.back().pTex.Get(), &SRVDesc, &renderTargets.back().pSRV);

	// store resolution in newly created render target entry
	renderTargets.back().width = width;
	renderTargets.back().height = height;

	// prepare viewport description
	renderTargets.back().vp.Width = width;
	renderTargets.back().vp.Height = height;
	renderTargets.back().vp.MinDepth = 0.0f;
	renderTargets.back().vp.MaxDepth = 1.0f;
	renderTargets.back().vp.TopLeftX = 0.0f;

	// return render target index
	return renderTargets.size() - 1;
}

int8_t D3DMgr::CreateDepthBuffer(uint8_t rtIndex) noexcept
{
	// if render target is out of bounds - return error
	if (rtIndex > renderTargets.size() - 1)
	{
		return -1;
	}

	// initialize new buffer entry
	depthTargets.push_back({});

	uint16_t width = RenderTargets()->at(rtIndex).width;
	uint16_t height = RenderTargets()->at(rtIndex).height;

	//create depth stencil texture
	D3D11_TEXTURE2D_DESC stencilDesc = {};
	stencilDesc.Usage = D3D11_USAGE_DEFAULT;
	stencilDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	stencilDesc.Width = width;
	stencilDesc.Height = height;
	stencilDesc.MipLevels = 1u;
	stencilDesc.ArraySize = 1u;
	stencilDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	stencilDesc.SampleDesc.Count = 1u;
	stencilDesc.SampleDesc.Quality = 0u;

	D3D_THROW_INFO(m_pDevice->CreateTexture2D(&stencilDesc, nullptr, &depthTargets.back().pDSTex));

	//create depth stencil view
	D3D11_DEPTH_STENCIL_VIEW_DESC dsvd = {};
	dsvd.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	dsvd.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	dsvd.Texture2D.MipSlice = 0u;

	D3D_THROW_INFO(m_pDevice->CreateDepthStencilView(depthTargets.back().pDSTex.Get(), &dsvd, &depthTargets.back().pDSV));

	depthTargets.back().width = width;
	depthTargets.back().height = height;

	return depthTargets.size() - 1;
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

void D3DMgr::RTInitBuffers(bool isHDR) noexcept
{
	// create render and depth buffers (at slot index 1)
	CreateDepthBuffer(CreateRenderBuffer(m_VWidth, m_VHeight, isHDR));

	// slot 2
	CreateRenderBuffer(m_VWidth, m_VHeight, isHDR);

	// slot 3 (smaller buffer resolution for post process effects)
	CreateDepthBuffer(CreateRenderBuffer(m_VWidth / 2, m_VHeight / 2, isHDR));
}

void D3DMgr::RTBind(uint8_t rtIndex, uint8_t dsIndex, uint8_t num) noexcept
{
	(rtIndex > renderTargets.size() - 1) ?
		MessageBoxA(nullptr, "Render target index is out of bounds.", "RTBind Error", MB_OK | MB_ICONERROR) : 0;
	(dsIndex > depthTargets.size() - 1) ?
		MessageBoxA(nullptr, "Depth buffer index is out of bounds.", "RTBind Error", MB_OK | MB_ICONERROR) : 0;

	D3D_THROW_INFO(m_pContext->OMSetRenderTargets(
		num, renderTargets[rtIndex].pRTV.GetAddressOf(), depthTargets[dsIndex].pDSV.Get())
	);

	D3D_THROW_INFO(m_pContext->RSSetViewports(1u, &renderTargets[rtIndex].vp));

	m_RTId = rtIndex;
	m_DSId = dsIndex;
	m_RTNum = num;
}

void D3DMgr::RTCopyBuffer(uint8_t indexFrom, uint8_t indexTo) noexcept
{
	// if index is out of bounds - copy nothing
	if (indexFrom < 1 || indexFrom > renderTargets.size() - 1 || indexTo > renderTargets.size() - 1)
	{
#ifdef _DEBUG || _DFDEBUG
		OutputDebugStringA("RTCopyBuffer Error: Index is out of bounds.\n");
#endif
		return;
	}
	//unbind render targets
	//Context()->OMSetRenderTargets(0u, nullptr, nullptr);

	//copy shader resource / texture
	Context()->CopyResource(renderTargets[indexTo].pTex.Get(), renderTargets[indexFrom].pTex.Get());

	//bind render targets
	//Context()->OMSetRenderTargets(m_RTNum, &RTV.m_pRTViews[m_RTId], RTV.m_pDSViews[m_RTId]);
}

std::vector<D3DMgr::RenderTarget>* D3DMgr::RenderTargets() noexcept
{
	return &renderTargets;
}

std::vector<D3DMgr::DSTarget>* D3DMgr::DepthTargets() noexcept
{
	return &depthTargets;
}

ID3D11RenderTargetView* D3DMgr::RTGetRTV(uint8_t index) noexcept
{
	if (index < renderTargets.size())
	{
		return renderTargets[index].pRTV.Get();
	}
}

ID3D11DepthStencilView* D3DMgr::RTGetDSV(uint8_t index) noexcept
{
	if (index < depthTargets.size())
	{
		return depthTargets[index].pDSV.Get();
	}
}

void D3DMgr::RTRemove(uint8_t index) noexcept
{
	if (index > 1 && index < renderTargets.size())
	{
		renderTargets.erase(renderTargets.begin() + index);
	}
}