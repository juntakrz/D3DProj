#pragma once

#include "IBind.h"

namespace Bind
{
	class Texture : public IBind
	{
		UINT m_slot = 0u;
		UINT m_num = 1u;

	public:
		Texture(std::shared_ptr<ID3D11ShaderResourceView*> m_pSRV, UINT slot = 0);
		Texture(ID3D11ShaderResourceView* pSRV, UINT slot);
		Texture(ID3D11ShaderResourceView* pSRVArray[], UINT slot, UINT num);
		void Bind() noexcept override;
		void Unbind() noexcept override;

	protected:
		std::shared_ptr<ID3D11ShaderResourceView*> m_pSRV = nullptr;
		ID3D11ShaderResourceView* m_pDirectSRV[4] = { nullptr, nullptr, nullptr, nullptr };
		ID3D11ShaderResourceView* nullSRV = nullptr;
	};
}