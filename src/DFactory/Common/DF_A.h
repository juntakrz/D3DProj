#pragma once

#include "../D3DMgr/D3DMgr.h"

namespace DF
{
	extern D3DMgr* pD3DM;
	ID3D11Device* Device() noexcept;
	ID3D11DeviceContext* Context() noexcept;
}