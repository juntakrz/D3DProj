#pragma once

#include "MeshBase.h"
#include "../Primitives/CPlane.h"

class MeshPlane : public MeshBase<MeshPlane>
{
public:
	MeshPlane(uint16_t matId, uint16_t divisionsX = 1, uint16_t divisionsY = 1);
};