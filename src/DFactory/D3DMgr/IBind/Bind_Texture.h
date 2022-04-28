#pragma once

#include "IBind.h"

namespace Bind
{
	class Texture : public IBind
	{
		UINT m_slot = 0u;

	public:
		Texture(const DFSurface& surface, UINT slot = 0);
		Texture(const COMPTR<ID3D11ShaderResourceView>* pSRV, UINT slot = 0);
		void Bind() noexcept override;

	protected:
		COMPTR<ID3D11ShaderResourceView> m_pSRV;
	};
}