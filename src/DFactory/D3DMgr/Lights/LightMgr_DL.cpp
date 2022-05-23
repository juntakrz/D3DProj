#include "LightMgr.h"

LightMgr::DirLight_ConstPSBuffer& LightMgr::DL() noexcept
{
	return dlData;
}

void LightMgr::DLSetCamera(CCamera* pCam) noexcept
{
	m_pDirCamera = pCam;
}

void LightMgr::DLVSBufferClear() noexcept
{
	//dlViewProj.DLViewProj.clear();
}

void LightMgr::DLVSBufferSetViewProj(uint8_t index) noexcept
{
	if (m_pDirCamera)
	{
		dlViewProj.DLViewProj[index] = m_pDirCamera->m_XMViewProj;
	}
}

void LightMgr::DLVSBufferBind() noexcept
{
	// update and bind directional light view buffer
	dirVSBuffer.Update(dlViewProj);
	dirVSBuffer.Bind();
}

void LightMgr::DLSetPos(const float& x, const float& y, const float& z) noexcept
{
	dlData.pos = { x, y, z };
}

void LightMgr::DLSetPos(const XMFLOAT3A& pos) noexcept
{
	dlData.pos = pos;
}

const XMFLOAT3& LightMgr::DLGetPos() const noexcept
{
	return dlData.pos;
}

const XMFLOAT3A& LightMgr::DLGetPosA() const noexcept
{
	return *(reinterpret_cast<const XMFLOAT3A*>(&dlData.pos));
}
