#pragma once

#include "MeshBase.h"
#include "../Primitives/CCube.h"

class MeshCube : public MeshBase<MeshCube>
{
public:
	MeshCube(D3DMgr& d3dMgr, std::string material, uint16_t paramA = 1, uint16_t paramB = 1);
	DirectX::XMMATRIX GetTransformXM() const noexcept override;
};