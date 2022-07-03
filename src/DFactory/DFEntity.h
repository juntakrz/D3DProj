#pragma once

class DFEntity
{
private:
	struct TransformData
	{
		XMFLOAT3A translation = { 0.0f, 0.0f, 0.0f };
		XMFLOAT3A rotation = { 0.0f, 0.0f, 0.0f };
		XMFLOAT3A scaling = { 1.0f, 1.0f, 1.0f };
	} transform;

public:
	virtual void SetPos(float x, float y, float z) noexcept = 0;
	virtual void SetPos(const XMFLOAT3A& pos) noexcept = 0;
	virtual void SetRotation(float x, float y, float z) noexcept = 0;
	virtual void SetRotation(const XMFLOAT3A& pos) noexcept = 0;
	virtual void SetScale(float x, float y, float z) noexcept = 0;
	virtual void SetScale(const XMFLOAT3A& pos) noexcept = 0;

	virtual TransformData& GetTransformData() noexcept = 0;
};