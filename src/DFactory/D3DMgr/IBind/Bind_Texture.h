#pragma once

#include "IBind.h"

namespace Bind
{
	class Texture : public IBind
	{
		UINT m_slot = 0u;

	public:
		Texture(D3DMgr& d3dMgr, const DFSurface& surface, UINT slot = 0);
		void Bind(D3DMgr& d3dMgr) noexcept override;

	protected:
		COMPTR<ID3D11ShaderResourceView> m_pSRV;
	};
}