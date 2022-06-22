#pragma once

#include "../D3DMgr/D3DMgr.h"
class DFactory;

namespace DF
{
	//extern void* Engine;
	extern DFactory* Engine;
	extern D3DMgr* D3DM;
	ID3D11Device* Device() noexcept;
	ID3D11DeviceContext* Context() noexcept;

	struct
	{
		const char* depthView = "dsCSM";
	} DEBUG;
}