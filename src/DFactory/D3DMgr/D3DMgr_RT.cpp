#include "D3DMgr.h"

void D3DMgr::RTCreateBuffer(uint8_t index, bool isHDR) noexcept
{
	if (index < 1 && index > maxRT - 1) return;

	//create texture to be used as a render target
	D3D11_TEXTURE2D_DESC texDesc{};
	isHDR ? 
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
	texDesc.MiscFlags = 0;

	Device()->CreateTexture2D(&texDesc, nullptr, &RT[index].pTex);

	D3D11_RENDER_TARGET_VIEW_DESC RTVDesc{};
	RTVDesc.Format = texDesc.Format;
	RTVDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
	RTVDesc.Texture2D.MipSlice = 0;

	Device()->CreateRenderTargetView(RT[index].pTex, &RTVDesc, &RT[index].pRTV);

	D3D11_SHADER_RESOURCE_VIEW_DESC SRVDesc{};
	SRVDesc.Format = texDesc.Format;
	SRVDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	SRVDesc.Texture2D.MipLevels = 1;
	SRVDesc.Texture2D.MostDetailedMip = 0;

	Device()->CreateShaderResourceView(RT[index].pTex, &SRVDesc, &RT[index].pSRV);
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
	//assign primary back and depth buffers
	RT[0].pRTV = m_pBackBuf_RT.Get();
	RT[0].pDSV = m_pBackBuf_DS.Get();

	//assign aux render to texture resources
	RTCreateBuffer(1, isHDR);
	RT[1].pDSV = m_pBackBuf_DS.Get();	//use the same depth buffer as primary by default
}

void D3DMgr::RTBindMainAndAux() noexcept
{
	D3D_THROW_INFO(m_pContext->OMSetRenderTargets(2u, &RT[0].pRTV, RT[0].pDSV));
	m_RTId = 0;
	m_RTNum = 2;
}

void D3DMgr::RTBindMain() noexcept
{
	D3D_THROW_INFO(m_pContext->OMSetRenderTargets(1u, &RT[0].pRTV, RT[0].pDSV));
	m_RTId = 0;
	m_RTNum = 1;
}

void D3DMgr::RTBindAux() noexcept
{
	D3D_THROW_INFO(m_pContext->OMSetRenderTargets(1u, &RT[1].pRTV, RT[1].pDSV));
	m_RTId = 1;
	m_RTNum = 1;
}

void D3DMgr::RTBind(uint8_t index, uint8_t num) noexcept
{
	D3D_THROW_INFO(m_pContext->OMSetRenderTargets(num, &RT[index].pRTV, RT[index].pDSV));
	m_RTId = index;
	m_RTNum = num;
}

void D3DMgr::RTCopyBuffer(uint8_t index) noexcept
{
	if (index < 1 && index > maxRT - 1) return;
	//unbind render targets
	//Context()->OMSetRenderTargets(0u, nullptr, nullptr);

	//copy shader resource
	Context()->CopyResource(m_pBufCopy_Tex.Get(), RT[index].pTex);

	//bind render targets
	//Context()->OMSetRenderTargets(m_RTNum, &RTV.m_pRTViews[m_RTId], RTV.m_pDSViews[m_RTId]);
}

D3DMgr::RenderTargets& D3DMgr::RTGet() noexcept
{
	return *RT;
}

ID3D11RenderTargetView* D3DMgr::RTGetRTV(uint8_t index) noexcept
{
	if (index < maxRT - 1)
	{
		return RT[index].pRTV;
	}
}

ID3D11DepthStencilView* D3DMgr::RTGetDSV(uint8_t index) noexcept
{
	if (index < maxRT - 1)
	{
		return RT[index].pDSV;
	}
}

void D3DMgr::RTSet(uint8_t index, ID3D11RenderTargetView* pRTV, ID3D11DepthStencilView* pDSV) noexcept
{
	if (index > 1 && index < maxRT - 1)
	{
		pRTV ? RT[index].pRTV = pRTV : 0;
		pDSV ? RT[index].pDSV = pDSV : 0;
	}
}

void D3DMgr::RTRemove(uint8_t index, bool release) noexcept
{
	if (index > 1 && index < maxRT - 1)
	{
		if(release)
		{
			SAFE_RELEASE(RT[index].pRTV);
			SAFE_RELEASE(RT[index].pDSV);
			SAFE_RELEASE(RT[index].pSRV);
			SAFE_RELEASE(RT[index].pTex);
		}
		else
		{
			RT[index].pRTV = nullptr;
			RT[index].pDSV = nullptr;
			RT[index].pSRV = nullptr;
			RT[index].pTex = nullptr;
		}
	}
}