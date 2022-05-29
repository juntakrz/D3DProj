#include "IBind.h"

namespace Bind
{
	ID3D11Device* IBind::GetDevice() noexcept
	{
		return DF::D3DM->Device();
	}

	ID3D11DeviceContext* IBind::GetContext() noexcept
	{
		return DF::D3DM->Context();
	}
}