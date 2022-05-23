#include "MeshCore.h"

void MeshCore::XMUpdate(FXMMATRIX transform) noexcept
{
	//receive transform from the node and apply it to the mesh transform
	xmMain = transform;

	DirectX::XMStoreFloat3A(&xmPos, DirectX::FXMVECTOR(xmMain.r[3]));

	// bounding sphere calculations
	if (m_calcBSphere)
	{
		m_BSphere.Center = *reinterpret_cast<XMFLOAT3*>(&xmPos);

		float scaleX = xmMain.r[0].m128_f32[0];
		float scaleY = xmMain.r[1].m128_f32[1];
		float scaleZ = xmMain.r[2].m128_f32[2];

		m_BSphere.Radius = (scaleX >= scaleY && scaleX >= scaleZ)
			? scaleX * m_radius
			: (scaleY >= scaleZ)
			? scaleY * m_radius : scaleZ * m_radius;

		m_calcBSphere = false;
	}
}

DirectX::XMMATRIX MeshCore::GetXMTransform() const noexcept
{
	return xmMain;
}

DirectX::XMMATRIX* MeshCore::GetMatrix() noexcept
{
	return &xmMain;
}

const DirectX::XMMATRIX* MeshCore::GetMatrix() const noexcept
{
	return &xmMain;
}

void MeshCore::SetMatrix(DirectX::FXMMATRIX& matrix) noexcept
{
	xmMain = std::move(matrix);
}

DirectX::XMFLOAT3A* MeshCore::GetXMPos() noexcept
{
	return &xmPos;
}

DirectX::BoundingSphere* MeshCore::GetXMSphere() noexcept
{
	return &m_BSphere;
}

void MeshCore::SetXMSphereRadius(const float& radius) noexcept
{
	m_radius = radius;
}

void MeshCore::CalcXMSphereBoundaries() noexcept
{
	m_calcBSphere = true;
}

void MeshCore::CalcMeshRadius(const std::vector<DF::Vertex>& inVertices, float& outRadius) noexcept
{
	// get initial vertex values
	XMFLOAT3 vMin = { inVertices[0].pos.x, inVertices[0].pos.y, inVertices[0].pos.z };
	XMFLOAT3 vMax = { inVertices[0].pos.x, inVertices[0].pos.y, inVertices[0].pos.z };

	// compare and get min and max values
	for (const auto& it : inVertices)
	{
		vMin.x = std::min(vMin.x, it.pos.x);
		vMin.y = std::min(vMin.y, it.pos.y);
		vMin.z = std::min(vMin.z, it.pos.z);

		vMax.x = std::max(vMax.x, it.pos.x);
		vMax.y = std::max(vMax.y, it.pos.y);
		vMax.z = std::max(vMax.z, it.pos.z);
	}

	// get radius
	(vMin.x < 0) ? vMin.x *= -1.0f : 0u;
	(vMin.y < 0) ? vMin.y *= -1.0f : 0u;
	(vMin.z < 0) ? vMin.z *= -1.0f : 0u;

	// determine which resulting vector is the longest and return it as a radius
	outRadius = std::max((vMin.x + vMin.y + vMin.z), (vMax.x + vMax.y + vMax.z));
}

void MeshCore::SetPos(float x, float y, float z) noexcept
{
	transform.translation = { x, y, z };
}

DirectX::XMFLOAT3A MeshCore::GetPos() noexcept
{
	return transform.translation;
}

void MeshCore::TranslateXYZ(float x, float y, float z) noexcept
{
	transform.translation.x += x;
	transform.translation.y += y;
	transform.translation.z += z;
}

void MeshCore::TranslateX(float x) noexcept
{
	transform.translation.x += x;
}

void MeshCore::TranslateY(float y) noexcept
{
	transform.translation.y += y;
}

void MeshCore::TranslateZ(float z) noexcept
{
	transform.translation.z += z;
}

void MeshCore::ScaleXYZ(float x, float y, float z) noexcept
{
	transform.scale.x = x;
	transform.scale.y = y;
	transform.scale.z = z;
}

void MeshCore::ScaleX(float x) noexcept
{
	transform.scale.x = x;
}

void MeshCore::ScaleY(float y) noexcept
{
	transform.scale.y = y;
}

void MeshCore::ScaleZ(float z) noexcept
{
	transform.scale.z = z;
}

void MeshCore::RotateXYZ(float x, float y, float z) noexcept
{
	transform.rotation = { x, y, -z };
}

void MeshCore::RotateX(float x) noexcept
{
	transform.rotation.x = x;
}

void MeshCore::RotateY(float y) noexcept
{
	transform.rotation.y = y;
}

void MeshCore::RotateZ(float z) noexcept
{
	transform.rotation.z = -z;
}