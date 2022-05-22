#include "Bind_Texture.h"

namespace Bind
{
	Texture::Texture(std::shared_ptr<ID3D11ShaderResourceView*> pSRV, UINT slot)
		: m_slot(slot)
	{
		m_pSRV = pSRV;
	}

	Texture::Texture(ID3D11ShaderResourceView* pSRV, UINT slot) : m_slot(slot)
	{
		m_pDirectSRV[0] = pSRV;
	}

	Texture::Texture(ID3D11ShaderResourceView* pSRVArray[], UINT slot, UINT num) : m_slot(slot), m_num(num)
	{
		if (m_num > 4)
		{
#ifdef _DEBUG || _DFDEBUG
			OutputDebugStringA("Bind Texture Error: number of shader resource views is out of bounds, limiting to 4.\n");
#endif
			m_num = 4;
		}

		for (uint8_t i = 0; i < m_num; i++)
		{
			(pSRVArray[i]) ? m_pDirectSRV[i] = pSRVArray[i] : m_pDirectSRV[i] = nullptr;
		}
	}

	void Texture::Bind() noexcept
	{
		(m_pDirectSRV[0])
			? GetContext()->PSSetShaderResources(m_slot, m_num, m_pDirectSRV)
			: GetContext()->PSSetShaderResources(m_slot, m_num, m_pSRV.get());
	}
	void Texture::Unbind() noexcept
	{
		ID3D11ShaderResourceView* nullSRV = NULL;
		GetContext()->PSSetShaderResources(m_slot, m_num, &nullSRV);
	}
}