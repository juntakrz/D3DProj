#include "MeshCore.h"

void MeshCore::XMUpdate(FXMMATRIX transform) noexcept
{
	// receive transform from the node and apply it to the mesh transform
	xmMain = transform;

	DirectX::XMStoreFloat3A(&xmPos, DirectX::FXMVECTOR(xmMain.r[3]));

	// calculate distance to camera using either the closest AABB coordinate or mesh origin if it's the nearest
	//if (m_hasOwnAABB)
	{
		XMVECTOR p0 = XMLoadFloat3(&m_AABBcoords[0]);
		XMVECTOR p1 = XMLoadFloat3(&m_AABBcoords[1]);

		p0 = XMVector3Transform(p0, xmMain);
		p1 = XMVector3Transform(p1, xmMain);

		XMVECTOR camPos = XMLoadFloat3(&DF::D3DM->Camera()->GetPos());

		XMVECTOR v0 = p0 - camPos;
		XMVECTOR v1 = p1 - camPos;

		XMFLOAT3 d0, d1;

		XMStoreFloat3(&d0, XMVector3Length(v0));
		XMStoreFloat3(&d1, XMVector3Length(v1));

		(d0.x < d1.x) ? m_distanceToCamera = d0.x : m_distanceToCamera = d1.x;
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

void MeshCore::CalcMeshAABBPoints(const std::vector<DF::Vertex>& vertices) noexcept
{
	// get initial vertex values
	XMFLOAT3 vMin = { vertices[0].pos.x, vertices[0].pos.y, vertices[0].pos.z };
	XMFLOAT3 vMax = { vertices[0].pos.x, vertices[0].pos.y, vertices[0].pos.z };

	// compare and get min and max values
	for (const auto& it : vertices)
	{
		vMin.x = std::min(vMin.x, it.pos.x);
		vMin.y = std::min(vMin.y, it.pos.y);
		vMin.z = std::min(vMin.z, it.pos.z);

		vMax.x = std::max(vMax.x, it.pos.x);
		vMax.y = std::max(vMax.y, it.pos.y);
		vMax.z = std::max(vMax.z, it.pos.z);
	}

	// return opposite points for AABB construction
	m_AABBcoords[0] = vMin;
	m_AABBcoords[1] = vMax;
}

const XMFLOAT3* MeshCore::AABBPoints() const noexcept
{
	return &m_AABBcoords[0];
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