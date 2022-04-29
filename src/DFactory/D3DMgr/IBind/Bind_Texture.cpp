#include "Bind_Texture.h"

namespace Bind
{
	/*
	Texture::Texture(const DFSurface& surface, UINT slot) : m_slot(slot)
	{
		D3D_DXGIDEBUG(*DFData::pD3DM);

		//creating 2D texture as a shader resource
		D3D11_TEXTURE2D_DESC texDesc = {};
		texDesc.Width = surface.GetWidth();
		texDesc.Height = surface.GetHeight();
		texDesc.MipLevels = 1;
		texDesc.ArraySize = 1;
		texDesc.SampleDesc.Count = 1;
		texDesc.SampleDesc.Quality = 0;
		texDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
		texDesc.Usage = D3D11_USAGE_DEFAULT;
		texDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
		texDesc.CPUAccessFlags = 0;
		texDesc.MiscFlags = 0;

		D3D11_SUBRESOURCE_DATA sd = {};
		sd.pSysMem = surface.GetBufferPtr();
		sd.SysMemPitch = surface.GetWidth() * sizeof(DFSurface::Color);	//memory distance between texture lines

		COMPTR<ID3D11Texture2D> pTexture;
		D3D_THROW(GetDevice()->CreateTexture2D(&texDesc, &sd, &pTexture));

		D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
		srvDesc.Format = texDesc.Format;
		srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		srvDesc.Texture2D.MipLevels = 1;
		srvDesc.Texture2D.MostDetailedMip = 0;

		D3D_THROW(GetDevice()->CreateShaderResourceView(pTexture.Get(), &srvDesc, &m_pSRV));
	}
	*/
	Texture::Texture(ID3D11ShaderResourceView* pSRV, UINT slot) : m_slot(slot)
	{
		D3D_DXGIDEBUG(*DFData::pD3DM);

		//m_pSRV = pSRV->Get();
		//pSRV->CopyTo(m_pSRV.GetAddressOf());
		m_pSRV = pSRV;
	}

	void Texture::Bind() noexcept
	{
		//GetContext()->PSSetShaderResources(m_slot, 1u, m_pSRV.GetAddressOf());
		GetContext()->PSSetShaderResources(m_slot, 1u, &m_pSRV);
	}
}