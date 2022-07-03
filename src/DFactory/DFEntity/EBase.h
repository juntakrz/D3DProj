#pragma once

/*
* Base class for manipulated engine objects like models, cameras and lights
* contains common variables and methods either used as is or overloaded by the inheriting classes
*/

class EBase
{
public:
	std::string name;

protected:
	struct TransformData {

		// data used in actual transformation calculations
		XMFLOAT3A translation = { 0.0f, 0.0f, 0.0f };
		XMFLOAT3A rotation = { 0.0f, 0.0f, 0.0f };
		XMFLOAT3A scaling = { 1.0f, 1.0f, 1.0f };

		struct {

			// initial data defined by Set* methods
			XMFLOAT3A translation = { 0.0f, 0.0f, 0.0f };
			XMFLOAT3A rotation = { 0.0f, 0.0f, 0.0f };
			XMFLOAT3A scaling = { 0.0f, 0.0f, 0.0f };
		} initial;

	} transform;

public:
	XMMATRIX xmMain = XMMatrixIdentity();

	virtual FXMMATRIX& GetXMTransform() noexcept = 0;
	virtual TransformData& GetTransformData() noexcept = 0;

	void SetPos(float x, float y, float z) noexcept;
	void SetPos(const XMFLOAT3A& pos) noexcept;
	XMFLOAT3A& GetPos() noexcept;

	void Move(float x = 0.0f, float y = 0.0f, float z = 0.0f) noexcept;
	void Move(const XMFLOAT3A& delta) noexcept;

	void SetRotation(float x = 0.0f, float y = 0.0f, float z = 0.0f) noexcept;
	void SetRotation(const XMFLOAT3A& rotation) noexcept;
	XMFLOAT3A& GetRotation() noexcept;

	void Rotate(float x = 0.0f, float y = 0.0f, float z = 0.0f) noexcept;
	void Rotate(const XMFLOAT3A& delta) noexcept;

	void SetScale(float x, float y, float z) noexcept;
	void SetScale(const XMFLOAT3A& scale) noexcept;
	XMFLOAT3A& GetScale() noexcept;

	void Scale(float x = 1.0f, float y = 1.0f, float z = 1.0f) noexcept;
	void Scale(const XMFLOAT3A& delta) noexcept;

	uint8_t TypeId();
};