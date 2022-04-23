#include "IBind.h"

namespace Bind
{
	ID3D11Device* IBind::GetDevice(D3DMgr& d3dMgr) noexcept
	{
		return d3dMgr.m_pDevice.Get();
	}

	ID3D11DeviceContext* IBind::GetContext(D3DMgr& d3dMgr) noexcept
	{
		return d3dMgr.m_pContext.Get();
	}
}