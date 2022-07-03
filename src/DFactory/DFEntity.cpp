#include "../pch.h"
#include "DFEntity.h"
#include "Common/DF_Math.h"

void DFEntity::SetPos(float x, float y, float z) noexcept
{
	transform.translation.x = x;
	transform.translation.y = y;
	transform.translation.z = z;
}

void DFEntity::SetPos(const XMFLOAT3A& pos) noexcept
{
	transform.translation = pos;
}

void DFEntity::AdjustPos(float x, float y, float z) noexcept
{
	transform.translation.x += x;
	transform.translation.y += y;
	transform.translation.z += z;
}

void DFEntity::AdjustPos(const XMFLOAT3A& pos) noexcept
{
	transform.translation.x += pos.x;
	transform.translation.y += pos.y;
	transform.translation.z += pos.z;
}

void DFEntity::SetRotation(float x, float y, float z) noexcept
{
	transform.rotation.x = GMath::WrapAngle(x);
	transform.rotation.y = GMath::WrapAngle(y);
	transform.rotation.z = GMath::WrapAngle(z);
}

void DFEntity::SetRotation(const XMFLOAT3A& rotation) noexcept
{
	transform.rotation = { GMath::WrapAngle(rotation.x), GMath::WrapAngle(rotation.y), GMath::WrapAngle(rotation.z) };
}

void DFEntity::SetScale(float x, float y, float z) noexcept
{
	transform.scaling.x = x;
	transform.scaling.y = y;
	transform.scaling.z = z;
}

void DFEntity::SetScale(const XMFLOAT3A& scale) noexcept
{
	transform.scaling = scale;
}
