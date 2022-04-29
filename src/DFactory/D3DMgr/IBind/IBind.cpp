#include "IBind.h"

namespace Bind
{
	ID3D11Device* IBind::GetDevice() noexcept
	{
		return DF::pD3DM->m_pDevice.Get();
	}

	ID3D11DeviceContext* IBind::GetContext() noexcept
	{
		return DF::pD3DM->m_pContext.Get();
	}
}