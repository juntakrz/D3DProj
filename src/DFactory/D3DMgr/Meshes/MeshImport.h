#pragma once

#include "MeshBase.h"

class MeshImport : public MeshBase<MeshImport>
{
private:
	mutable XMFLOAT4X4 transform;

public:
	// get binds and vertices for boundaries calculations from assimp mesh
	MeshImport(std::vector<std::unique_ptr<Bind::IBind>> pBinds, const std::vector<DF::Vertex>& vertices);
};