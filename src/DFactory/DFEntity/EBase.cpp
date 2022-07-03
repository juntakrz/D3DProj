#include "../pch.h"
#include "EBase.h"
#include "../Common/DF_Math.h"

void EBase::SetPos(float x, float y, float z) noexcept
{
	transform.translation.x = x;
	transform.translation.y = y;
	transform.translation.z = z;

	transform.initial.translation = transform.translation;
}

void EBase::SetPos(const XMFLOAT3A& pos) noexcept
{
	transform.translation = pos;
	transform.initial.translation = transform.translation;
}

XMFLOAT3A& EBase::GetPos() noexcept
{
	return transform.translation;
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

	transform.initial.rotation = transform.rotation;
}

void EBase::SetRotation(const XMFLOAT3A& rotation) noexcept
{
	transform.rotation = { GMath::WrapAngle(rotation.x), GMath::WrapAngle(rotation.y), GMath::WrapAngle(rotation.z) };
	transform.initial.rotation = transform.rotation;
}

XMFLOAT3A& EBase::GetRotation() noexcept
{
	return transform.rotation;
}

void EBase::Rotate(float x, float y, float z) noexcept
{
	transform.rotation.x = GMath::WrapAngle(transform.rotation.x + x);
	transform.rotation.y = GMath::WrapAngle(transform.rotation.x + y);
	transform.rotation.z = GMath::WrapAngle(transform.rotation.x + z);
}

void EBase::Rotate(const XMFLOAT3A& delta) noexcept
{
	transform.rotation.x = GMath::WrapAngle(transform.rotation.x + delta.x);
	transform.rotation.y = GMath::WrapAngle(transform.rotation.x + delta.y);
	transform.rotation.z = GMath::WrapAngle(transform.rotation.x + delta.z);
}

void EBase::SetScale(float x, float y, float z) noexcept
{
	transform.scaling.x = x;
	transform.scaling.y = y;
	transform.scaling.z = z;

	transform.initial.scaling = transform.scaling;
}

void EBase::SetScale(const XMFLOAT3A& scale) noexcept
{
	transform.scaling = scale;
	transform.initial.scaling = transform.scaling;
}

XMFLOAT3A& EBase::GetScale() noexcept
{
	return transform.scaling;
}

void EBase::Scale(float x, float y, float z) noexcept
{
	transform.scaling.x *= x;
	transform.scaling.y *= y;
	transform.scaling.z *= z;
}

void EBase::Scale(const XMFLOAT3A& delta) noexcept
{
	transform.scaling.x *= delta.x;
	transform.scaling.y *= delta.y;
	transform.scaling.z *= delta.z;
}

uint8_t EBase::TypeId()
{
	return 0;
}
