#include "DF_A.h"

namespace DF
{
	D3DMgr* pD3DM = nullptr;
	void* Engine = nullptr;

	ID3D11Device* Device() noexcept
	{
		return pD3DM->Device();
	}

	ID3D11DeviceContext* Context() noexcept
	{
		return pD3DM->Context();
	}
}