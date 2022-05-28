#include "DFModelMgr.h"

void DFModelMgr::SetPos(float x, float y, float z) noexcept
{
	m_Models[m_curModel].transform.translation = { x, y, z };
}

void DFModelMgr::SetRotation(float x, float y, float z) noexcept
{
	m_Models[m_curModel].transform.rotation = { GMath::WrapAngle(x), GMath::WrapAngle(y), GMath::WrapAngle(-z) };
}

void DFModelMgr::SetScaling(float x, float y, float z) noexcept
{
	m_Models[m_curModel].transform.scaling = { x, y, z };
}

FXMMATRIX DFModelMgr::DFModel::GetModelXMTransform() noexcept
{
	return (m_followCamera)
		? XMMatrixScaling(transform.scaling.x, transform.scaling.y, transform.scaling.z)
		* XMMatrixRotationRollPitchYaw(GMath::WrapAngle(transform.rotation.x), GMath::WrapAngle(transform.rotation.y), GMath::WrapAngle(transform.rotation.z))
		* XMMatrixTranslation(
			transform.translation.x + DF::Engine->CameraGetActive().second->GetPos().x,
			transform.translation.y + DF::Engine->CameraGetActive().second->GetPos().y,
			transform.translation.z + DF::Engine->CameraGetActive().second->GetPos().z)
		: XMMatrixScaling(transform.scaling.x, transform.scaling.y, transform.scaling.z)
		* XMMatrixRotationRollPitchYaw(GMath::WrapAngle(transform.rotation.x), GMath::WrapAngle(transform.rotation.y), GMath::WrapAngle(transform.rotation.z))
		* XMMatrixTranslation(transform.translation.x, transform.translation.y, transform.translation.z);
}