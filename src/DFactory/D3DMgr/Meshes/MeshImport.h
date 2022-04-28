#pragma once

#include "MeshBase.h"

class MeshImport : public MeshBase<MeshImport>
{
private:
	mutable XMFLOAT4X4 transform;

public:
	MeshImport(std::vector<std::unique_ptr<Bind::IBind>> pBinds);
	XMMATRIX GetTransformXM() const noexcept override;
};