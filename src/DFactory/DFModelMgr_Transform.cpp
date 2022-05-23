#include "DFModelMgr.h"

void DFModelMgr::SetPos(float x, float y, float z) noexcept
{
	m_Models[m_curModel].transform.translation = { x, y, z };
	m_Models[m_curModel].calcBoundaries = true;		// recalculate boundaries after every shape change
}

void DFModelMgr::SetRotation(float x, float y, float z) noexcept
{
	m_Models[m_curModel].transform.rotation = { GMath::WrapAngle(x), GMath::WrapAngle(y), GMath::WrapAngle(-z) };
}

void DFModelMgr::SetScaling(float x, float y, float z) noexcept
{
	m_Models[m_curModel].transform.scaling = { x, y, z };
	m_Models[m_curModel].calcBoundaries = true;
}

FXMMATRIX DFModelMgr::DFModel::GetModelXMTransform() noexcept
{
	
	return XMMatrixScaling(transform.scaling.x, transform.scaling.y, transform.scaling.z)
		* XMMatrixRotationRollPitchYaw(GMath::WrapAngle(transform.rotation.x), GMath::WrapAngle(transform.rotation.y), GMath::WrapAngle(transform.rotation.z))
		* XMMatrixTranslation(transform.translation.x, transform.translation.y, transform.translation.z);
}