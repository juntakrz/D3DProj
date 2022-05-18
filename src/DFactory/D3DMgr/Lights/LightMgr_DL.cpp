#include "LightMgr.h"

LightMgr::DirLight_ConstPSBuffer& LightMgr::DL() noexcept
{
	return dlData;
}

void LightMgr::DLSetCamera(CCamera* pCam) noexcept
{
	m_pDirCamera = pCam;
}

void LightMgr::DLSetViewData() noexcept
{
	if (m_pDirCamera)
	{
		dlViewProj.DLView = XMMatrixTranspose(m_pDirCamera->m_XMView);
		dlViewProj.DLProj = XMMatrixTranspose(m_pDirCamera->m_XMProj);
	}
}

void LightMgr::DLSetPos(float x, float y, float z) noexcept
{
	dlData.pos = { x, y, z };
}

void LightMgr::DLSetPos(XMFLOAT3 pos) noexcept
{
	dlData.pos = pos;
}