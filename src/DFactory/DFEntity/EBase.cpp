#include "../pch.h"
#include "EBase.h"
#include "../Common/DF_Math.h"

void EBase::SetPos(float x, float y, float z) noexcept
{
	transform.translation.x = x;
	transform.translation.y = y;
	transform.translation.z = z;
}

void EBase::SetPos(const XMFLOAT3A& pos) noexcept
{
	transform.translation = pos;
}

void EBase::Move(float x, float y, float z) noexcept
{
	transform.translation.x += x;
	transform.translation.y += y;
	transform.translation.z += z;
}

void EBase::Move(const XMFLOAT3A& delta) noexcept
{
	transform.translation.x += delta.x;
	transform.translation.y += delta.y;
	transform.translation.z += delta.z;
}

void EBase::SetRotation(float x, float y, float z) noexcept
{
	transform.rotation.x = GMath::WrapAngle(x);
	transform.rotation.y = GMath::WrapAngle(y);
	transform.rotation.z = GMath::WrapAngle(z);
}

void EBase::SetRotation(const XMFLOAT3A& rotation) noexcept
{
	transform.rotation = { GMath::WrapAngle(rotation.x), GMath::WrapAngle(rotation.y), GMath::WrapAngle(rotation.z) };
}

void EBase::SetScale(float x, float y, float z) noexcept
{
	transform.scaling.x = x;
	transform.scaling.y = y;
	transform.scaling.z = z;
}

void EBase::SetScale(const XMFLOAT3A& scale) noexcept
{
	transform.scaling = scale;
}

const uint8_t& EBase::TypeId() const noexcept
{
	return m_typeId;
}
