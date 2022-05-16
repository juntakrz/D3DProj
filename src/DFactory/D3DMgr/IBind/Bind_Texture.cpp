#include "Bind_Texture.h"

namespace Bind
{
	Texture::Texture(std::shared_ptr<ID3D11ShaderResourceView*> pSRV, UINT slot)
		: m_slot(slot)
	{
		m_pSRV = pSRV;
	}

	Texture::Texture(ID3D11ShaderResourceView* m_pSRV, UINT slot) : m_slot(slot), m_pDirectSRV(m_pSRV)
	{
	}

	void Texture::Bind() noexcept
	{
		(m_pDirectSRV)
			? GetContext()->PSSetShaderResources(m_slot, 1u, &m_pDirectSRV)
			: GetContext()->PSSetShaderResources(m_slot, 1u, m_pSRV.get());
	}
	void Texture::Unbind() noexcept
	{
		ID3D11ShaderResourceView* nullSRV = NULL;
		GetContext()->PSSetShaderResources(m_slot, 1u, &nullSRV);
	}
}