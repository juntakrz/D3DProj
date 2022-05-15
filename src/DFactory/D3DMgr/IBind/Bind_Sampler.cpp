#include "Bind_Sampler.h"

namespace Bind
{
	Sampler::Sampler(Mode mode)
	{
		D3D_DXGIDEBUG(*DF::D3DM);

		// enum type conversion
		D3D11_TEXTURE_ADDRESS_MODE texAddressMode = (D3D11_TEXTURE_ADDRESS_MODE)mode;

		D3D11_SAMPLER_DESC samplerDesc = {};
		samplerDesc.Filter = D3D11_FILTER_ANISOTROPIC;
		samplerDesc.AddressU = texAddressMode;
		samplerDesc.AddressV = texAddressMode;
		samplerDesc.AddressW = texAddressMode;
		samplerDesc.MinLOD = 0.0f;
		samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;
		samplerDesc.MaxAnisotropy = D3D11_REQ_MAXANISOTROPY;

		D3D_THROW(GetDevice()->CreateSamplerState(&samplerDesc, &m_pSampler));
	}

	void Sampler::Bind() noexcept
	{
		GetContext()->PSSetSamplers(0u, 1u, m_pSampler.GetAddressOf());
	}
}