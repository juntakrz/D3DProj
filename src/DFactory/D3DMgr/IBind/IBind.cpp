#include "IBind.h"

namespace Bind
{
	ID3D11Device* IBind::GetDevice() noexcept
	{
		return DF::DFM->m_pDevice.Get();
	}

	ID3D11DeviceContext* IBind::GetContext() noexcept
	{
		return DF::DFM->m_pContext.Get();
	}
}