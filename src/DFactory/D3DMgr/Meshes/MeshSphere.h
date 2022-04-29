#pragma once

#include "MeshBase.h"
#include "../Primitives/CSphere.h"

class MeshSphere : public MeshBase<MeshSphere>
{
public:
	MeshSphere(uint16_t matId = 0, uint16_t divisions = 32, bool invertFaces = false);
	DirectX::XMMATRIX GetTransformXM() const noexcept override;
};