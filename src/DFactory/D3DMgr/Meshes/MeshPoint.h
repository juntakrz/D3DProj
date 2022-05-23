#pragma once

#include "MeshBase.h"

class MeshPoint : public MeshBase<MeshPoint>
{
public:
	MeshPoint(uint16_t matId, float scale) noexcept;
};