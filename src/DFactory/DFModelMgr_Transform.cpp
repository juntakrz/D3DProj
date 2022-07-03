#include "DFModelMgr.h"

void DFModelMgr::SetPos(float x, float y, float z) noexcept
{
	m_Models[m_curModel].SetPos(x, y, z);
}

void DFModelMgr::SetRotation(float x, float y, float z) noexcept
{
	m_Models[m_curModel].SetRotation(x, y, z);
}

void DFModelMgr::SetScaling(float x, float y, float z) noexcept
{
	m_Models[m_curModel].SetScale(x, y, z);
}