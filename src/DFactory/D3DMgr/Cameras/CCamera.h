#pragma once

#include "../../../pch.h"

using namespace DirectX;

class CCamera
{
private:
	
	float m_deltaMove = 1.0f;
	float m_deltaRotation = 0.03f;

	//set vectors

	XMFLOAT3 m_pos = { 0.0f, 0.0f, 0.0f };
	XMFLOAT3 m_rot = { 0.0f, 0.0f, 0.0f };
	XMVECTOR m_vecUp = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
	XMVECTOR m_vecFwd = XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f);

	//processed vectors
	XMVECTOR m_vecPos, m_vecFocus;

public:
	XMMATRIX m_XMView;

public:
	CCamera(const float posX = 0.0f, const float posY = 0.0f, const float posZ = 0.0f) noexcept;
	~CCamera() = default;

	void SetView() noexcept;
	
	void SetPos(float posX = 0.0f, float posY = 0.0f, float posZ = 0.0f) noexcept;
	void SetPos(XMFLOAT3 pos) noexcept;
	const XMFLOAT3& GetPos() const noexcept;

	void SetMovementDelta(float delta) noexcept;
	const float& GetMovementDelta() const noexcept;

	void SetRotationDelta(float delta) noexcept;
	const float& GetRotationDelta() const noexcept;

	void Move(float x = 0.0f, float y = 0.0f, float z = 0.0f) noexcept;

	void Rotate(float pitch = 0.0f, float yaw = 0.0f, float roll = 0.0f) noexcept;
	void Rotate(const XMFLOAT3& rotation) noexcept;
	void RotateAdd(float pitch = 0.0f, float yaw = 0.0f, float roll = 0.0f) noexcept;
	void RotateAdd(const XMFLOAT3& rotation) noexcept;

	void SetUpDirection(float dirX = 0.0f, float dirY = 0.0f, float dirZ = 0.0f) noexcept;
};