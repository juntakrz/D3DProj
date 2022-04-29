#include "D3DMgr.h"

void D3DMgr::CreateAuxRenderTarget() noexcept
{
	//create texture to be used as a render target

	D3D11_TEXTURE2D_DESC AuxTexDesc{};
	AuxTexDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
	AuxTexDesc.Width = m_VWidth;
	AuxTexDesc.Height = m_VHeight;
	AuxTexDesc.MipLevels = 1;
	AuxTexDesc.ArraySize = 1;
	AuxTexDesc.SampleDesc.Count = 1;
	AuxTexDesc.Usage = D3D11_USAGE_DEFAULT;
	AuxTexDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;
	AuxTexDesc.CPUAccessFlags = 0;
	AuxTexDesc.MiscFlags = 0;

	Device()->CreateTexture2D(&AuxTexDesc, nullptr, &m_pAuxRTTex);

	D3D11_RENDER_TARGET_VIEW_DESC RTVDesc{};
	RTVDesc.Format = AuxTexDesc.Format;
	RTVDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
	RTVDesc.Texture2D.MipSlice = 0;

	Device()->CreateRenderTargetView(m_pAuxRTTex.Get(), &RTVDesc, &m_pRTVAux);

	D3D11_SHADER_RESOURCE_VIEW_DESC SRVDesc{};
	SRVDesc.Format = AuxTexDesc.Format;
	SRVDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	SRVDesc.Texture2D.MipLevels = 1;
	SRVDesc.Texture2D.MostDetailedMip = 0;

	Device()->CreateShaderResourceView(m_pAuxRTTex.Get(), &SRVDesc, &m_pSRVAux);
}

void D3DMgr::AssignInitPtrs() noexcept
{
	//assign primary back and depth buffers
	RTV.m_pRTViews[0] = m_pRTVMain.Get();
	RTV.m_pDSViews[0] = m_pDSVMain.Get();

	//assign aux render to texture resources
	RTV.m_pRTViews[1] = m_pRTVAux.Get();
	RTV.m_pSRViews[1] = m_pSRVAux.Get();
	RTV.m_pDSViews[1] = m_pDSVMain.Get();	//use the same depth buffer as primary by default
}

void D3DMgr::RTBindMainAndAux() noexcept
{
	D3D_THROW_INFO(m_pContext->OMSetRenderTargets(2u, &RTV.m_pRTViews[0], RTV.m_pDSViews[0]));
	m_RTId = 0;
	m_RTNum = 2;
}

void D3DMgr::RTBindMain() noexcept
{
	D3D_THROW_INFO(m_pContext->OMSetRenderTargets(1u, &RTV.m_pRTViews[0], RTV.m_pDSViews[0]));
	m_RTId = 0;
	m_RTNum = 1;
}

void D3DMgr::RTBindAux() noexcept
{
	D3D_THROW_INFO(m_pContext->OMSetRenderTargets(1u, &RTV.m_pRTViews[1], RTV.m_pDSViews[1]));
	m_RTId = 1;
	m_RTNum = 1;
}

void D3DMgr::RTBind(uint8_t index, uint8_t num) noexcept
{
	(index == 255 && num == 255) ?
		index = m_RTId, num = m_RTNum :
		(index > 1 && index < RTV.maxRTVs - 1 && num > 0 && num < RTV.maxRTVs - index - 1) ?
			m_RTId = index, m_RTNum = num : index = 0, num = 1;

	D3D_THROW_INFO(m_pContext->OMSetRenderTargets(num, &RTV.m_pRTViews[index], RTV.m_pDSViews[index]));
}

void D3DMgr::RTCopyBuffer() noexcept
{
	//unbind render targets
	//Context()->OMSetRenderTargets(0u, nullptr, nullptr);

	//copy shader resource
	Context()->CopyResource(m_pAuxRTTex.Get(), m_pBackBuffer.Get());

	//bind render targets
	//Context()->OMSetRenderTargets(m_RTNum, &RTV.m_pRTViews[m_RTId], RTV.m_pDSViews[m_RTId]);
}

D3DMgr::RenderTargets& D3DMgr::RTGet() noexcept
{
	return RTV;
}

ID3D11RenderTargetView* D3DMgr::RTGetRTV(uint8_t index) noexcept
{
	if (index < RTV.maxRTVs - 1)
	{
		return RTV.m_pRTViews[index];
	}
}

ID3D11DepthStencilView* D3DMgr::RTGetDSV(uint8_t index) noexcept
{
	if (index < RTV.maxRTVs - 1)
	{
		return RTV.m_pDSViews[index];
	}
}

void D3DMgr::RTAdd(uint8_t index, ID3D11RenderTargetView* pRTV, ID3D11DepthStencilView* pDSV) noexcept
{
	if (index > 1 && index < RTV.maxRTVs - 1)
	{
		RTV.m_pRTViews[index] = pRTV;
		RTV.m_pDSViews[index] = pDSV;
	}
}

void D3DMgr::RTAddRTV(uint8_t index, ID3D11RenderTargetView* pRTV) noexcept
{
	if (index > 1 && index < RTV.maxRTVs - 1)
	{
		RTV.m_pRTViews[index] = pRTV;
	}
}

void D3DMgr::RTAddDSV(uint8_t index, ID3D11DepthStencilView* pDSV) noexcept
{
	if (index > 1 && index < RTV.maxRTVs - 1)
	{
		RTV.m_pDSViews[index] = pDSV;
	}
}

void D3DMgr::RTRemove(uint8_t index, bool free) noexcept
{
	if (index > 1 && index < RTV.maxRTVs - 1)
	{
		if (free)
		{
			RTV.m_pRTViews[index]->Release();
			RTV.m_pDSViews[index]->Release();
		}

		RTV.m_pRTViews[index] = nullptr;
		RTV.m_pDSViews[index] = nullptr;
	}
}

void D3DMgr::RTRemoveRTV(uint8_t index, bool free) noexcept
{
	if (index > 1 && index < RTV.maxRTVs - 1)
	{
		if (free)
		{
			RTV.m_pRTViews[index]->Release();
		}

		RTV.m_pRTViews[index] = nullptr;
	}
}

void D3DMgr::RTRemoveDSV(uint8_t index, bool free) noexcept
{
	if (index > 1 && index < RTV.maxRTVs - 1)
	{
		if (free)
		{
			RTV.m_pDSViews[index]->Release();
		}

		RTV.m_pDSViews[index] = nullptr;
	}
}