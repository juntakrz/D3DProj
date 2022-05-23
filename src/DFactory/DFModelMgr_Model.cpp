#include "DFModelMgr.h"

void DFModelMgr::DFModel::SetPos(const XMFLOAT3A& position) noexcept
{
	transform.translation = position;
}

void DFModelMgr::DFModel::SetPos(const float& x, const float& y, const float& z) noexcept
{
	transform.translation = { x, y, z };
}

void DFModelMgr::DFModel::AdjustPos(const XMFLOAT3A& position) noexcept
{
	transform.translation.x += position.x;
	transform.translation.y += position.y;
	transform.translation.z += position.z;
}

void DFModelMgr::DFModel::AdjustPos(const float& x, const float& y, const float& z) noexcept
{
	transform.translation.x += x;
	transform.translation.y += y;
	transform.translation.z += z;
}

void DFModelMgr::DFModel::FollowCamera(const bool& enabled) noexcept
{
	m_followCamera = enabled;
}