#pragma once

#include "MeshBase.h"
#include "../Primitives/CSphere.h"

class MeshPointLight : public MeshBase<MeshPointLight>
{
private:
	bool m_showMesh = true;

	struct
	{
		mutable XMFLOAT3A rotation = { 0.0f, 0.0f, 0.0f };
	} mod;

public:
	static bool m_showAllMeshes;

	MeshPointLight(D3DMgr& d3dMgr);

	void EnableLightMesh() noexcept;
	void DisableLightMesh() noexcept;

	void Draw(D3DMgr& d3dMgr) noexcept;

	void DEBUG_Rotate(float delta = 0.0f) noexcept;

	DirectX::XMMATRIX GetTransformXM() const noexcept override;
};