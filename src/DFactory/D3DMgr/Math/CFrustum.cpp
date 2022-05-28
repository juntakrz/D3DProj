#include "../../pch.h"
#include "CFrustum.h"

// static declarations
DirectX::BoundingFrustum CFrustum::m_BFrustum;
XMVECTOR CFrustum::m_planes[6];

void CFrustum::CalcPlanes(CXMMATRIX transform, const bool& transpose) noexcept
{
	XMMATRIX matrix;
	(transpose) ? matrix = XMMatrixTranspose(transform) : matrix = transform;

	// near plane
	m_planes[0] = matrix.r[2];

	// far plane
	m_planes[1] = matrix.r[3] - matrix.r[2];

	// left plane
	m_planes[2] = matrix.r[3] + matrix.r[0];

	// right plane
	m_planes[3] = matrix.r[3] - matrix.r[0];

	// top plane
	m_planes[4] = matrix.r[3] - matrix.r[1];

	// bottom plane
	m_planes[5] = matrix.r[3] + matrix.r[1];

	for (auto& it : m_planes)
	{
		it = XMPlaneNormalize(it);
	}
}

const bool& CFrustum::TestSphere(const XMFLOAT3& centerPos, const float& radius) noexcept
{
	return true;
}

void CFrustum::CalcFrustum(CXMMATRIX view, CXMMATRIX proj) noexcept
{
	XMMATRIX inverseView = XMMatrixInverse(nullptr, view);
	BoundingFrustum::CreateFromMatrix(m_BFrustum, proj);
	m_BFrustum.Transform(m_BFrustum, inverseView);
}

DirectX::BoundingFrustum* CFrustum::Frustum() noexcept
{
	return &m_BFrustum;
}
