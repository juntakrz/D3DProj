#include "DF_A.h"

namespace DF
{
	D3DMgr* DFM = nullptr;
	void* Engine = nullptr;

	ID3D11Device* Device() noexcept
	{
		return DFM->Device();
	}

	ID3D11DeviceContext* Context() noexcept
	{
		return DFM->Context();
	}
}