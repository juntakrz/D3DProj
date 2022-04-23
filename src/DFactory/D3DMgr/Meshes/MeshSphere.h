#pragma once

#include "MeshBase.h"
#include "../Primitives/CSphere.h"

class MeshSphere : public MeshBase<MeshSphere>
{
public:
	MeshSphere(D3DMgr& d3dMgr, std::string material, uint16_t divisions = 32, bool invertFaces = false);
	DirectX::XMMATRIX GetTransformXM() const noexcept override;
};