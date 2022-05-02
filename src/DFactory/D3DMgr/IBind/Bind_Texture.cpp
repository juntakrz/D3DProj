#include "Bind_Texture.h"

namespace Bind
{
	Texture::Texture(std::shared_ptr<ID3D11ShaderResourceView*> pSRV, UINT slot)
		: m_slot(slot)
	{
		m_pSRV = pSRV;
	}

	void Texture::Bind() noexcept
	{
		GetContext()->PSSetShaderResources(m_slot, 1u, m_pSRV.get());
	}
	void Texture::Unbind() noexcept
	{
		ID3D11ShaderResourceView* nullSRV = NULL;
		GetContext()->PSSetShaderResources(m_slot, 1u, &nullSRV);
	}
}