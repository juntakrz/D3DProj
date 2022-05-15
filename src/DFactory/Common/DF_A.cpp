#include "DF_A.h"

namespace DF
{
	D3DMgr* D3DM = nullptr;
	DFactory* Engine = nullptr;
	//void* Engine = nullptr;

	ID3D11Device* Device() noexcept
	{
		return D3DM->Device();
	}

	ID3D11DeviceContext* Context() noexcept
	{
		return D3DM->Context();
	}
}