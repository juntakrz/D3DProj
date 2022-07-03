#include "../../pch.h"
#include "EModel.h"
#include "../DFactory.h"
#include "../Common/DF_A.h"

FXMMATRIX& EModel::GetXMTransform() noexcept
{
	return (m_followCamera)
		? XMMatrixScaling(transform.scaling.x, transform.scaling.y, transform.scaling.z)
		* XMMatrixRotationRollPitchYaw(transform.rotation.x, transform.rotation.y, transform.rotation.z)
		* XMMatrixTranslation(
			transform.translation.x + DF::Engine->CameraGetActive().second->GetPos().x,
			transform.translation.y + DF::Engine->CameraGetActive().second->GetPos().y,
			transform.translation.z + DF::Engine->CameraGetActive().second->GetPos().z)
		: XMMatrixScaling(transform.scaling.x, transform.scaling.y, transform.scaling.z)
		* XMMatrixRotationRollPitchYaw(transform.rotation.x, transform.rotation.y, transform.rotation.z)
		* XMMatrixTranslation(transform.translation.x, transform.translation.y, transform.translation.z);
}

EModel::TransformData& EModel::GetTransformData() noexcept
{
	return transform;
}

uint8_t EModel::TypeId()
{
	return 1;
}

void EModel::FollowCamera(const bool& enabled) noexcept
{
	m_followCamera = enabled;
}