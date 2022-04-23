#include "DFactory.h"

void DFactory::AddCamera(std::string name) noexcept
{
	for (const auto& it : m_Cameras)
	{
		if (it.name == name) return;
	}

	m_Cameras.push_back({ name, std::make_unique<CCamera>(0.0f, 0.0f, 0.0f)});
}

void DFactory::AddCamera(std::string name, DirectX::XMFLOAT3& pos) noexcept
{
	for (const auto& it : m_Cameras)
	{
		if (it.name == name) return;
	}

	m_Cameras.push_back({ name, std::make_unique<CCamera>(-pos.x, -pos.y, -pos.z )});
}

void DFactory::AddCamera(std::string name, float x, float y, float z) noexcept
{
	for (const auto& it : m_Cameras)
	{
		if (it.name == name) return;
	}

	m_Cameras.push_back({ name, std::make_unique<CCamera>(-x, -y, -z) });
}

void DFactory::SetActiveCamera(std::string name, bool select) noexcept
{
	uint16_t index = 0;
	for (const auto& it : m_Cameras)
	{
		if (it.name == name)
		{
			vars.activeCamera = index;
			if (select)
			{
				vars.selectedCamera = index;
			}
		};
		index++;
	}
}

void DFactory::SetActiveCamera(uint16_t index, bool select) noexcept
{
	if (index < m_Cameras.size())
	{
		vars.activeCamera = index;
		if (select)
		{
			vars.selectedCamera = index;
		}
	}
}

void DFactory::SelectCamera(std::string name) noexcept
{
	uint16_t index = 0;
	for (const auto& it : m_Cameras)
	{
		if (it.name == name)
		{
			vars.selectedCamera = index;
		};
		index++;
	}
}

void DFactory::SelectCamera(uint16_t index) noexcept
{
	if (index < m_Cameras.size())
	{
		vars.selectedCamera = index;
	}
}

void DFactory::CameraBindVS() noexcept
{
	cameraConstBuffer.pos.x = m_Cameras[vars.activeCamera].pCam->GetPos().x;
	cameraConstBuffer.pos.y = m_Cameras[vars.activeCamera].pCam->GetPos().y;
	cameraConstBuffer.pos.z = m_Cameras[vars.activeCamera].pCam->GetPos().z;

	pCamCBuf = std::make_unique<Bind::ConstVertexBuffer<CameraConstVSBuffer>>(*pD3DMgr, cameraConstBuffer, 1u);
}

void DFactory::CameraUpdateVS() noexcept
{
	cameraConstBuffer.pos.x = m_Cameras[vars.activeCamera].pCam->GetPos().x;
	cameraConstBuffer.pos.y = m_Cameras[vars.activeCamera].pCam->GetPos().y;
	cameraConstBuffer.pos.z = m_Cameras[vars.activeCamera].pCam->GetPos().z;

	pCamCBuf->Bind(*pD3DMgr);
	pCamCBuf->Update(*pD3DMgr, cameraConstBuffer);
}

CCamera* DFactory::Camera() noexcept
{
	return m_Cameras[vars.selectedCamera].pCam.get();
}