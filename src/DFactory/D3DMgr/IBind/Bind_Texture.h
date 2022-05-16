#pragma once

#include "IBind.h"

namespace Bind
{
	class Texture : public IBind
	{
		UINT m_slot = 0u;

	public:
		Texture(std::shared_ptr<ID3D11ShaderResourceView*> m_pSRV, UINT slot = 0);
		Texture(ID3D11ShaderResourceView* m_pSRV, UINT slot);
		void Bind() noexcept override;
		void Unbind() noexcept override;

	protected:
		std::shared_ptr<ID3D11ShaderResourceView*> m_pSRV = nullptr;
		ID3D11ShaderResourceView* m_pDirectSRV = nullptr;
		ID3D11ShaderResourceView* nullSRV = nullptr;
	};
}