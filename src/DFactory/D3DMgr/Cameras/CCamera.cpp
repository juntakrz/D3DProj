#include "../../Common/DF_A.h"
#include "CCamera.h"

CCamera::CCamera(const float posX, const float posY, const float posZ) noexcept
	: m_pos({ posX, posY, posZ })
{
	SetAsPerspective(1.0f, DF::D3DM->GetAspectRatio(), 0.05f, 5000.05f);
	m_orthoData = { 1.0f * 32.0f, 9.0f/16.0f * 32.0f, 0.05f, 100.05f };
}

void CCamera::SetView() noexcept
{
	m_vecPos = XMLoadFloat3(&m_pos);

	switch (m_lookAt)
	{
	case true:
	{
		/*
		// add X/Y rotated Z direction and position vectors to get adjusted view direction vector
		m_vecFocus = m_vecPos + XMVector3Transform(
			m_vecFwd,
			XMMatrixRotationRollPitchYaw(-m_rot.x, -m_rot.y, 0.0f)
		);*/

		if (m_followCam)
		{
			
		}

		m_XMView = XMMatrixLookAtLH(m_vecPos, XMLoadFloat3(&m_targetPos), m_vecUp);
		return;
	}
	case false:
	{
		m_vecFocus = XMVector3Transform(
			m_vecFwd,
			XMMatrixRotationRollPitchYaw(-m_rot.x, -m_rot.y, 0.0f)
		);

		m_XMView = XMMatrixLookToLH(m_vecPos, m_vecFocus, m_vecUp);
		return;
	}
	}
}

void CCamera::SetViewProj() noexcept
{
	m_XMViewProj = XMMatrixTranspose(XMMatrixMultiply(m_XMView, m_XMProj));
}

void CCamera::EnableLookAt() noexcept
{
	m_lookAt = true;
}

void CCamera::DisableLookAt() noexcept
{
	m_lookAt = false;
}

void CCamera::LockTo(CCamera* pCam) noexcept
{
	m_followCam = pCam;
	m_lookAt = true;
}

void CCamera::LookAt(float x, float y, float z) noexcept
{
	m_targetPos = { x, y, z };
}

void CCamera::LookAt(std::string objectId) noexcept
{
	m_targetId = objectId;
}

void CCamera::SetAsPerspective() noexcept
{
	m_XMProj = XMMatrixPerspectiveFovLH(m_perspData.x, m_perspData.y, m_perspData.z, m_perspData.w);
}

void CCamera::SetAsPerspective(float FOV, float aspectRatio, float nearZ, float farZ) noexcept
{
	m_perspData = { FOV, aspectRatio, nearZ, farZ };
	SetAsPerspective();
}

void CCamera::SetAsOrthographic() noexcept
{
	m_XMProj = XMMatrixOrthographicLH(m_orthoData.x, m_orthoData.y, m_orthoData.z, m_orthoData.w);
}

void CCamera::SetAsOrthographic(float width, float height, float nearZ, float farZ) noexcept
{
	m_orthoData = { width, height, nearZ, farZ };
	SetAsOrthographic();
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

void CCamera::SetRotation(int pitchDeg, int yawDeg) noexcept
{
	// limit pitch angle
	(pitchDeg < -90) ? pitchDeg = -90 : (pitchDeg > 90) ? pitchDeg = 90 : pitchDeg;

	m_rot.x = GMath::WrapAngle(XMConvertToRadians((float)pitchDeg));
	m_rot.y = -GMath::WrapAngle(XMConvertToRadians((float)yawDeg));
}

void CCamera::SetRotation(float pitchRad, float yawRad) noexcept
{
	m_rot.x = GMath::WrapAngle(pitchRad);
	m_rot.y = -GMath::WrapAngle(yawRad);
}

const XMFLOAT3& CCamera::GetInitialPos() const noexcept
{
	return m_pos;
}

const XMFLOAT3& CCamera::GetPos() const noexcept
{
	return m_pos;
	//return m_adjPos;
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
