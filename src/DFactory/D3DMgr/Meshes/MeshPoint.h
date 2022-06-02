#pragma once

#include "MeshBase.h"

class MeshPoint : public MeshBase<MeshPoint>
{
public:
	MeshPoint(uint16_t matId) noexcept;

	// override to bind only a vertex buffer
	void BindCore() const noexcept override;

	// override to draw only a single vertex
	void DrawIndexed() noexcept override;
};