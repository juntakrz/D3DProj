#include "DFactory.h"

void DFactory::CameraAdd(std::string name, float x, float y, float z) noexcept
{
	// if camera exists - exit
	if (m_Cameras.find(name) == m_Cameras.end())
	{
		// create camera
		m_Cameras[name] = std::make_unique<CCamera>(-x, -y, -z);
	}
}

void DFactory::CameraAdd(std::string name, DirectX::XMFLOAT3& pos) noexcept
{
	// if camera exists - exit
	if (m_Cameras.find(name) == m_Cameras.end())
	{
		// create camera
		m_Cameras[name] = std::make_unique<CCamera>(-pos.x, -pos.y, -pos.z);
	}
}

void DFactory::CameraActivate(std::string name, bool select) noexcept
{
	// if camera does not exist - exit
	if (m_Cameras.find(name) != m_Cameras.end())
	{
		// activate camera
		vars.activeCamera = name;

		// select camera if required
		if (select)
		{
			vars.selectedCamera = name;
		}

		pD3DMgr->SetCamera(m_Cameras.at(name).get());
	}
}

void DFactory::CameraSelect(std::string name) noexcept
{
	// if camera does not exist - exit
	if (m_Cameras.find(name) != m_Cameras.end())
	{
		// mark camera as selected
		vars.selectedCamera = name;
	}
}


void DFactory::CameraBindVS() noexcept
{
	cameraConstBuffer.pos.x = m_Cameras.at(vars.activeCamera)->GetPos().x;
	cameraConstBuffer.pos.y = m_Cameras.at(vars.activeCamera)->GetPos().y;
	cameraConstBuffer.pos.z = m_Cameras.at(vars.activeCamera)->GetPos().z;

	pCamCBuf = std::make_unique<Bind::ConstVertexBuffer<CameraConstVSBuffer>>(cameraConstBuffer, 1u);
}

void DFactory::CameraUpdateVS() noexcept
{
	cameraConstBuffer.pos.x = m_Cameras.at(vars.activeCamera)->GetPos().x;
	cameraConstBuffer.pos.y = m_Cameras.at(vars.activeCamera)->GetPos().y;
	cameraConstBuffer.pos.z = m_Cameras.at(vars.activeCamera)->GetPos().z;

	pCamCBuf->Bind();
	pCamCBuf->Update(cameraConstBuffer);
}

CCamera* DFactory::Camera(const std::string& name) noexcept
{
	if (name == "")
	{
		return m_Cameras.at(vars.selectedCamera).get();
	}

	if (m_Cameras.find(name) != m_Cameras.end())
	{
		return m_Cameras.at(name).get();
	}

#ifdef _DEBUG || _DFDEBUG
	std::string str = "WARNING! DF Camera: '" + name + "' not found! Returning nullptr.\n";
	OutputDebugStringA(str.c_str());
#endif

	//return m_Cameras.at(vars.selectedCamera).get();
	return nullptr;
}

std::pair<std::string, CCamera*> DFactory::CameraGetActive() noexcept
{
	return std::pair<std::string, CCamera*>(vars.activeCamera, m_Cameras.at(vars.activeCamera).get());
}
