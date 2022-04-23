#include "CCamera.h"

CCamera::CCamera(const float posX, const float posY, const float posZ) noexcept
	: m_pos({ posX, posY, posZ })
{
}

void CCamera::SetView() noexcept
{
	m_vecPos = XMLoadFloat3(&m_pos);

	// add X/Y rotated Z direction and position vectors to get adjusted view direction vector
	m_vecFocus = m_vecPos + XMVector3Transform(
		m_vecFwd,
		XMMatrixRotationRollPitchYaw(-m_rot.x, -m_rot.y, 0.0f)
	);

	m_XMView = XMMatrixLookAtLH(m_vecPos, m_vecFocus, m_vecUp);
}

void CCamera::SetPos(float posX, float posY, float posZ) noexcept
{
	m_pos.x = posX;
	m_pos.y = posY;
	m_pos.z = posZ;
}

void CCamera::SetPos(XMFLOAT3 pos) noexcept
{
	m_pos = pos;
}

const XMFLOAT3& CCamera::GetPos() const noexcept
{
	return m_pos;
}

void CCamera::SetMovementDelta(float delta) noexcept
{
	m_deltaMove = delta;
}

const float& CCamera::GetMovementDelta() const noexcept
{
	return m_deltaMove;
}

void CCamera::SetRotationDelta(float delta) noexcept
{
	m_deltaRotation = delta;
}

const float& CCamera::GetRotationDelta() const noexcept
{
	return m_deltaRotation;
}

void CCamera::Move(float x, float y, float z) noexcept
{
	/* FPS CAMERA MOVEMENT FORMULA
	* 
	* [new_pos] = old_pos + speed_factor * transform(dir_vec, rot_matrix(-radians.x, -radians.y, 0))
	* 
	*/

	XMStoreFloat3(
		&m_pos,														// [new_pos]
		XMLoadFloat3(&m_pos)										// old_pos
		+ m_deltaMove												// speed_factor
		* XMVector3Transform(
			XMVectorSet(x, y, z, 0.0f),								// dir_vec
			XMMatrixRotationRollPitchYaw(-m_rot.x, -m_rot.y, 0.0f)	// rot_matrix
		)
	);
}

void CCamera::Rotate(float pitch, float yaw, float roll) noexcept
{
	m_rot.x = pitch;
	m_rot.y = yaw;
	m_rot.z = roll;
}

void CCamera::Rotate(const XMFLOAT3& rotation) noexcept
{
	m_rot = rotation;
}

void CCamera::RotateAdd(float pitch, float yaw, float roll) noexcept
{
	m_rot.x + pitch > DirectX::XM_PIDIV2 - 0.01f ? m_rot.x = DirectX::XM_PIDIV2 - 0.01f :
		m_rot.x + pitch < -DirectX::XM_PIDIV2 + 0.01f ? m_rot.x = -DirectX::XM_PIDIV2 + 0.01f : m_rot.x += pitch;

	m_rot.y += yaw;
	m_rot.z += roll;
}

void CCamera::RotateAdd(const XMFLOAT3& rotation) noexcept
{
	m_rot.x + rotation.x > DirectX::XM_PIDIV2 - 0.01f ? m_rot.x = DirectX::XM_PIDIV2 - 0.01f :
		m_rot.x + rotation.x < -DirectX::XM_PIDIV2 + 0.01f ? m_rot.x = -DirectX::XM_PIDIV2 + 0.01f : m_rot.x += rotation.x;

	m_rot.y += rotation.y;
	m_rot.z += rotation.z;
}

void CCamera::SetUpDirection(float dirX, float dirY, float dirZ) noexcept
{
	m_vecUp = XMVectorSet(dirX, dirY, dirZ, 0.0f);
}
