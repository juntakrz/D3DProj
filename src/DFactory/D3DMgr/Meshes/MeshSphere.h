#pragma once

#include "MeshBase.h"
#include "../Primitives/CSphere.h"

class MeshSphere : public MeshBase<MeshSphere>
{
public:
	MeshSphere(const char* matId, uint16_t divisions = 32, bool invertFaces = false);
};