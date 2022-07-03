#include "../../pch.h"
#include "ECamera.h"
#include "../Common/DF_A.h"

ECamera::ECamera() noexcept
{
	SetAsPerspective(1.0f, DF::D3DM->GetAspectRatio(), 0.05f, 400.05f);
	m_orthoData = { 16.0f, 9.0f, 0.05f, 120.0f };
}

void ECamera::Move(float x, float y, float z) noexcept
{
	/* FPS CAMERA MOVEMENT FORMULA
	*
	* [new_pos] = old_pos + speed_factor * transform(dir_vec, rot_matrix(-radians.x, -radians.y, 0))
	*
	*/

	XMStoreFloat3(
		&transform.translation,										// [new_pos]
		XMLoadFloat3(&transform.translation)						// old_pos
		+ m_deltaMove												// speed_factor
		* XMVector3Transform(
			XMVectorSet(x, y, z, 0.0f),								// dir_vec
			XMMatrixRotationRollPitchYaw(							// rot_matrix (usually z = 0.0)
				-transform.rotation.x, -transform.rotation.y, -transform.rotation.z
			)
		)
	);
}

void ECamera::Move(const XMFLOAT3A& delta) noexcept
{
	ECamera::Move(delta.x, delta.y, delta.z);
}

void ECamera::SetAsPerspective() noexcept
{
	m_XMProj = XMMatrixPerspectiveFovLH(m_perspData.x, m_perspData.y, m_perspData.z, m_perspData.w);
}

void ECamera::SetAsPerspective(float FOV, float aspectRatio, float nearZ, float farZ) noexcept
{
	m_perspData = { FOV, aspectRatio, nearZ, farZ };
	SetAsPerspective();
}

void ECamera::SetAsOrthographic() noexcept
{
	m_XMProj = XMMatrixOrthographicLH(m_orthoData.x, m_orthoData.y, m_orthoData.z, m_orthoData.w);
	//m_XMProj = XMMatrixOrthographicOffCenterLH(-m_orthoData.x, m_orthoData.x, -m_orthoData.y, m_orthoData.y, m_orthoData.z, m_orthoData.w);
}

void ECamera::SetAsOrthographic(float width, float height, float nearZ, float farZ) noexcept
{
	m_orthoData = { width, height, nearZ, farZ };
	SetAsOrthographic();
}