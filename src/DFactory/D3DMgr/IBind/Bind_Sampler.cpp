#include "Bind_Sampler.h"

namespace Bind
{
	Sampler::Sampler(D3DMgr& d3dMgr)
	{
		D3D_DXGIDEBUG(d3dMgr);

		D3D11_SAMPLER_DESC samplerDesc = {};
		samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
		samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
		samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
		samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;

		D3D_THROW(GetDevice(d3dMgr)->CreateSamplerState(&samplerDesc, &m_pSampler));
	}

	void Sampler::Bind(D3DMgr& d3dMgr) noexcept
	{
		GetContext(d3dMgr)->PSSetSamplers(0u, 1u, m_pSampler.GetAddressOf());
	}
}