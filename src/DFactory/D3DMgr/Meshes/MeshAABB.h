#pragma once

#include "MeshBase.h"
#include "../Primitives/CCube.h"

class MeshAABB : public MeshBase<MeshAABB>
{
public:
	MeshAABB(const XMFLOAT3* AABBcoords);
};