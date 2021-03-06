#pragma once

#include "../../../pch.h"
#include "../../Common/DF_Math.h"

using namespace DirectX;

class CCamera
{
private:
	
	float m_deltaMove = 1.0f;
	float m_deltaRotation = 0.03f;

	// set vectors
	XMFLOAT3A m_pos = { 0.0f, 0.0f, 0.0f };		// camera position
	XMFLOAT3A m_initPos = { 0.0f, 0.0f, 0.0f };	// initial position
	XMFLOAT3A m_rot = { 0.0f, 0.0f, 0.0f };
	XMVECTOR m_vecUp = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
	XMVECTOR m_vecFwd = XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f);

	// processed vectors
	XMVECTOR m_vecPos, m_vecFocus;

	// view mode - lookAt object or lookTo direction
	bool m_lookAt = false;
	std::string m_targetId = "";					// if empty - target pos is used
	CCamera* m_pTargetCam = nullptr;				// if nullptr - won't follow
	XMFLOAT4 m_perspData;
	XMFLOAT4 m_orthoData;


public:
	XMMATRIX m_XMView;
	XMMATRIX m_XMProj;
	XMMATRIX m_XMViewProj;	// transposed view*proj matrix

public:
	CCamera(const float posX = 0.0f, const float posY = 0.0f, const float posZ = 0.0f) noexcept;
	~CCamera() = default;

	void SetView() noexcept;						// calculate camera view
	void SetViewProj() noexcept;					// calculate view*proj matrix

	void EnableLookAt() noexcept;	
	void DisableLookAt() noexcept;
	void LockToCameraTarget(CCamera* pCam) noexcept;	// nullptr to stop following
	void LookAt(float x, float y, float z) noexcept;
	void LookAt(std::string objectId) noexcept;
	const XMFLOAT3A& GetFocus() const noexcept;
	const XMVECTOR& GetFocusVector() const noexcept;

	void SetAsPerspective() noexcept;
	void SetAsPerspective(float FOV, float aspectRatio, float nearZ, float farZ) noexcept;
	void SetAsOrthographic() noexcept;
	void SetAsOrthographic(float width, float height, float nearZ, float farZ) noexcept;
	
	void SetPos(float posX = 0.0f, float posY = 0.0f, float posZ = 0.0f) noexcept;
	void SetPos(const XMFLOAT3A& pos) noexcept;
	void SetRotation(int pitchDeg, int yawDeg) noexcept;
	void SetRotation(float pitchRad = 0.0f, float yawRad = 0.0f) noexcept;

	const XMFLOAT3A& GetPos() const noexcept;
	const XMVECTOR& GetUpVector() const noexcept;

	void SetMovementDelta(float delta) noexcept;
	const float& GetMovementDelta() const noexcept;

	void SetRotationDelta(float delta) noexcept;
	const float& GetRotationDelta() const noexcept;

	void Move(float x = 0.0f, float y = 0.0f, float z = 0.0f) noexcept;

	void Rotate(float pitch = 0.0f, float yaw = 0.0f, float roll = 0.0f) noexcept;
	void Rotate(const XMFLOAT3A& rotation) noexcept;
	void RotateAdd(float pitch = 0.0f, float yaw = 0.0f, float roll = 0.0f) noexcept;
	void RotateAdd(const XMFLOAT3A& rotation) noexcept;

	void SetUpDirection(float dirX = 0.0f, float dirY = 0.0f, float dirZ = 0.0f) noexcept;
};