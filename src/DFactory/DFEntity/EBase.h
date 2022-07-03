#pragma once

class EBase
{
private:
	const uint8_t m_typeId = 0;

public:
	std::string name;

protected:
	struct TransformData
	{
		XMFLOAT3A translation = { 0.0f, 0.0f, 0.0f };
		XMFLOAT3A rotation = { 0.0f, 0.0f, 0.0f };
		XMFLOAT3A scaling = { 1.0f, 1.0f, 1.0f };
	} transform;

public:
	XMMATRIX xmMain = XMMatrixIdentity();

	virtual FXMMATRIX& GetXMTransform() noexcept = 0;
	virtual TransformData& GetTransformData() noexcept = 0;

	void SetPos(float x, float y, float z) noexcept;
	void SetPos(const XMFLOAT3A& pos) noexcept;

	void Move(float x = 0.0f, float y = 0.0f, float z = 0.0f) noexcept;
	void Move(const XMFLOAT3A& delta) noexcept;

	void SetRotation(float x = 0.0f, float y = 0.0f, float z = 0.0f) noexcept;
	void SetRotation(const XMFLOAT3A& rotation) noexcept;

	void SetScale(float x, float y, float z) noexcept;
	void SetScale(const XMFLOAT3A& scale) noexcept;

	const uint8_t& TypeId() const noexcept;
};