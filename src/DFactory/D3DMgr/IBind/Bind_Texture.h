#pragma once

#include "IBind.h"

namespace Bind
{
	class Texture : public IBind
	{
		UINT m_slot = 0u;

	public:
		//Texture(const DFSurface& surface, UINT slot = 0);
		Texture(ID3D11ShaderResourceView* m_pSRV, UINT slot = 0);
		void Bind() noexcept override;

	protected:
		//COMPTR<ID3D11ShaderResourceView> m_pSRV;
		ID3D11ShaderResourceView* m_pSRV = nullptr;
	};
}