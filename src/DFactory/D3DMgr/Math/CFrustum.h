#pragma once

class CFrustum
{
public:
	// get camera frustum planes from its view * projection matrix (once per frame)
	static void CalcPlanes(DirectX::CXMMATRIX transform, const bool& transpose = true) noexcept;

	// get center position of the mesh and its radius and detect if it's in view frustum (sphere testing)
	static const bool& TestSphere(const DirectX::XMFLOAT3& centerPos, const float& radius) noexcept;

	// enable or disable testing
	static void SetFrustumCulling(const bool& enabled) noexcept;

	// calculate frustum using DirectX Math SIMD library
	static void CalcFrustum(DirectX::CXMMATRIX view, DirectX::CXMMATRIX proj) noexcept;
	static DirectX::BoundingFrustum* Frustum() noexcept;

private:
	// camera frustum planes
	static DirectX::BoundingFrustum m_BFrustum;
	static DirectX::XMVECTOR m_planes[6];
	static bool m_cullingEnabled;
};