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

	MeshPointLight();

	void EnableLightMesh() noexcept;
	void DisableLightMesh() noexcept;

	void DrawIndexed() noexcept;

	void DEBUG_Rotate(float delta = 0.0f) noexcept;

	DirectX::XMMATRIX GetXMTransform() const noexcept override;
};