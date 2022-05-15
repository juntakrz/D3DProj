#include "LightMgr.h"

LightMgr::DirLight_ConstPSBuffer& LightMgr::DL() noexcept
{
	return dlData;
}

void LightMgr::DLSetViewMatrix() noexcept
{
	DirectX::XMFLOAT3 lookAt = { 0.0f, 0.0f, 0.0f };		// ?
	dlViewProj.DLView = XMMatrixTranspose(XMMatrixLookAtLH(DirectX::XMLoadFloat3(&dlData.pos), DirectX::XMLoadFloat3(&lookAt), m_vecUp));
}

void LightMgr::DLSetProjMatrix(float nearZ, float farZ) noexcept
{
	dlViewProj.DLProj = XMMatrixTranspose(XMMatrixPerspectiveFovLH(m_FOV, DF::D3DM->GetAspectRatio(), nearZ, farZ));
}