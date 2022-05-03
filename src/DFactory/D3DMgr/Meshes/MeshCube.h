#pragma once

#include "MeshBase.h"
#include "../Primitives/CCube.h"

class MeshCube : public MeshBase<MeshCube>
{
public:
	MeshCube(uint16_t matId, uint16_t paramA = 1, uint16_t paramB = 1);
};