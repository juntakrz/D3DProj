#pragma once

class EBase
{
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

	void AdjustPos(float x = 0.0f, float y = 0.0f, float z = 0.0f) noexcept;
	void AdjustPos(const XMFLOAT3A& pos) noexcept;

	void SetRotation(float x, float y, float z) noexcept;
	void SetRotation(const XMFLOAT3A& rotation) noexcept;

	void SetScale(float x, float y, float z) noexcept;
	void SetScale(const XMFLOAT3A& scale) noexcept;
};