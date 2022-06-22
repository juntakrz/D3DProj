#include "Bind_Sampler.h"

namespace Bind
{
	Sampler::Sampler(UINT mode, UINT slot, UINT target)
		:
		m_slot(slot), m_target(target)
	{
		D3D_DXGIDEBUG(*DF::D3DM);

		D3D11_SAMPLER_DESC samplerDesc = {};

		if (mode == 10u)
		{
			samplerDesc.Filter = D3D11_FILTER_COMPARISON_MIN_MAG_LINEAR_MIP_POINT;
			samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
			samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
			samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
			samplerDesc.MinLOD = 0.0f;
			samplerDesc.MaxLOD = 0.0f;
			samplerDesc.MaxAnisotropy = 0.0f;
			samplerDesc.ComparisonFunc = D3D11_COMPARISON_LESS;
		}
		else
		{
			// enum type conversion
			D3D11_TEXTURE_ADDRESS_MODE texAddressMode = (D3D11_TEXTURE_ADDRESS_MODE)mode;

			samplerDesc.Filter = D3D11_FILTER_ANISOTROPIC;
			samplerDesc.AddressU = texAddressMode;
			samplerDesc.AddressV = texAddressMode;
			samplerDesc.AddressW = texAddressMode;
			samplerDesc.MinLOD = 0.0f;
			samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;
			samplerDesc.MaxAnisotropy = D3D11_REQ_MAXANISOTROPY;
		};

		D3D_THROW(GetDevice()->CreateSamplerState(&samplerDesc, &m_pSampler));
	}

	void Sampler::Bind() noexcept
	{
		(m_target == DF::ShaderType::PS)
			? GetContext()->PSSetSamplers(m_slot, 1u, m_pSampler.GetAddressOf())
			: GetContext()->GSSetSamplers(m_slot, 1u, m_pSampler.GetAddressOf());
	}
}