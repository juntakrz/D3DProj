#pragma once

#include "EBase.h"

class ECamera : public EBase
{
private:
	const uint8_t m_typeId = 2;

	float m_deltaMove = 1.0f;
	float m_deltaRotation = 0.03f;

	// camera specific vectors: up and forward directions
	XMVECTOR m_vecUp = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
	XMVECTOR m_vecFwd = XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f);

	// processed vectors
	XMVECTOR m_vecPos, m_vecFocus;

	// view data
	XMFLOAT4 m_perspData;
	XMFLOAT4 m_orthoData;

	// view mode - lookAt object or lookTo direction
	bool m_lookAt = false;
	std::string m_targetId = "";					// if empty - target pos is used
	ECamera* m_pTargetCam = nullptr;				// if nullptr - won't follow

public:
	XMMATRIX m_XMView;
	XMMATRIX m_XMProj;
	XMMATRIX m_XMViewProj;	// transposed view*proj matrix

public:
	ECamera() noexcept;
	~ECamera() = default;

	void Move(float x = 0.0f, float y = 0.0f, float z = 0.0f) noexcept;
	void Move(const XMFLOAT3A& delta) noexcept;

	void SetAsPerspective() noexcept;
	void SetAsPerspective(float FOV, float aspectRatio, float nearZ, float farZ) noexcept;
	void SetAsOrthographic() noexcept;
	void SetAsOrthographic(float width, float height, float nearZ, float farZ) noexcept;

	void SetView() noexcept;						// calculate camera view
	void SetViewProj() noexcept;					// calculate view*proj matrix

	void EnableLookAt() noexcept;
	void DisableLookAt() noexcept;
	void LockToCameraTarget(ECamera* pCam) noexcept;	// nullptr to stop following
	void LookAt(float x, float y, float z) noexcept;
	void LookAt(std::string objectId) noexcept;
	const XMFLOAT3A& GetFocus() const noexcept;
	const XMVECTOR& GetFocusVector() const noexcept;

	const XMVECTOR& GetUpVector() const noexcept;

	void SetMovementDelta(float delta) noexcept;
	const float& GetMovementDelta() const noexcept;

	void SetRotationDelta(float delta) noexcept;
	const float& GetRotationDelta() const noexcept;

	void Rotate(float pitch = 0.0f, float yaw = 0.0f, float roll = 0.0f) noexcept;
	void Rotate(const XMFLOAT3A& rotation) noexcept;
	void RotateAdd(float pitch = 0.0f, float yaw = 0.0f, float roll = 0.0f) noexcept;
	void RotateAdd(const XMFLOAT3A& rotation) noexcept;

	void SetUpDirection(float dirX = 0.0f, float dirY = 0.0f, float dirZ = 0.0f) noexcept;
};