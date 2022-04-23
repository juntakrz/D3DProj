#pragma once

#include "MeshBase.h"
#include "../Primitives/CPlane.h"

class MeshPlane : public MeshBase<MeshPlane>
{
public:
	MeshPlane(D3DMgr& d3dMgr, std::string material, uint16_t divisionsX = 1, uint16_t divisionsY = 1);
	DirectX::XMMATRIX GetTransformXM() const noexcept override;
};