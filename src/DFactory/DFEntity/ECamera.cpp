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

void ECamera::Rotate(float x, float y, float z) noexcept
{
	// x = pitch, y = yaw, z = roll

	// limit pitching to -90 ... 90 degrees
	transform.rotation.x + x > DirectX::XM_PIDIV2 - 0.01f ? transform.rotation.x = DirectX::XM_PIDIV2 - 0.01f :
		transform.rotation.x + x < -DirectX::XM_PIDIV2 + 0.01f ? transform.rotation.x = -DirectX::XM_PIDIV2 + 0.01f : transform.rotation.x += x;

	transform.rotation.y += y;
	transform.rotation.z += z;
}

void ECamera::Rotate(const XMFLOAT3A& delta) noexcept
{
	// x = pitch, y = yaw, z = roll

	// limit pitching to -90 ... 90 degrees
	transform.rotation.x + delta.x > DirectX::XM_PIDIV2 - 0.01f ? transform.rotation.x = DirectX::XM_PIDIV2 - 0.01f :
		transform.rotation.x + delta.x < -DirectX::XM_PIDIV2 + 0.01f ? transform.rotation.x = -DirectX::XM_PIDIV2 + 0.01f : transform.rotation.x += delta.x;

	transform.rotation.y += delta.y;
	transform.rotation.z += delta.z;
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

void ECamera::SetView() noexcept
{
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

		if (m_pTargetCam)
		{
			XMFLOAT3 targetPos = m_pTargetCam->GetPos();
			XMFLOAT3 targetFocus = m_pTargetCam->GetFocus();
			XMVECTOR adjPos;

			// adjust position in relation to target camera
			adjPos = XMVectorAdd(XMLoadFloat3(&transform.initial.translation), XMLoadFloat3(&targetPos));
			adjPos = XMVectorAdd(adjPos, XMLoadFloat3(&targetFocus));
			XMStoreFloat3(&transform.translation, adjPos);

			// adjust focus point in relation to target camera's focus point
			m_vecFocus = XMVectorAdd(XMLoadFloat3(&targetPos), XMLoadFloat3(&targetFocus));
		}

		m_vecPos = XMLoadFloat3(&transform.translation);
		m_XMView = XMMatrixLookAtLH(m_vecPos, m_vecFocus, m_vecUp);
		return;
	}
	case false:
	{
		m_vecFocus = XMVector3Transform(
			m_vecFwd,
			XMMatrixRotationRollPitchYaw(-transform.rotation.x, -transform.rotation.y, -transform.rotation.z)
		);

		m_vecPos = XMLoadFloat3(&transform.translation);
		m_XMView = XMMatrixLookToLH(m_vecPos, m_vecFocus, m_vecUp);
		return;
	}
	}
}

void ECamera::SetViewProj() noexcept
{
	m_XMViewProj = XMMatrixTranspose(XMMatrixMultiply(m_XMView, m_XMProj));
}

// WIP
void ECamera::LookAt(float x, float y, float z) noexcept
{
	XMFLOAT3 focusPoint = { x, y, z };
	XMLoadFloat3(&focusPoint);
}

void ECamera::LookAt(std::string objectId) noexcept
{
	m_targetId = objectId;
}
//

bool ECamera::LookAtCameraTarget(ECamera* pCam) noexcept
{
	if (pCam)
	{
		m_pTargetCam = pCam;
		m_lookAt = true;
		return true;
	}

	m_lookAt = false;
	return false;
}

void ECamera::EnableLookAt() noexcept
{
	m_lookAt = true;
}

void ECamera::DisableLookAt() noexcept
{
	m_lookAt = false;
}

const XMFLOAT3A& ECamera::GetFocus() const noexcept
{
	XMFLOAT3A focus;
	XMStoreFloat3A(&focus, m_vecFocus);
	return focus;
}

const XMVECTOR& ECamera::GetFocusVector() const noexcept
{
	return m_vecFocus;
}

void ECamera::SetUpVector(float x, float y, float z) noexcept
{
	m_vecUp = XMVectorSet(x, y, z, 0.0f);
}

const XMVECTOR& ECamera::GetUpVector() const noexcept
{
	return m_vecUp;
}

void ECamera::SetMovementDelta(float delta) noexcept
{
	m_deltaMove = delta;
}

const float& ECamera::GetMovementDelta() const noexcept
{
	return m_deltaMove;
}

void ECamera::SetRotationDelta(float delta) noexcept
{
	m_deltaRotation = delta;
}

const float& ECamera::GetRotationDelta() const noexcept
{
	return m_deltaRotation;
}

uint8_t ECamera::TypeId()
{
	return 2;
}