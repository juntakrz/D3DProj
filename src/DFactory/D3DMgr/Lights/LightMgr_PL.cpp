#include "LightMgr.h"

uint16_t LightMgr::PLAdd(std::string name, float x, float y, float z) noexcept {

	// return point light index if already exists
	uint16_t index = 0;
	for (const auto& it : m_PLights) {
		if (it.name == name) {
			m_selPLight = index;
			return index;
		}
		index++;
	}

	// create new light and returning its index
	PLight newPL{};
	newPL.name = name;
	newPL.color = { 1.0f, 1.0f, 1.0f, 1.0f };
	newPL.pMesh = std::make_unique<MeshPointLight>(*m_d3dMgr);
	newPL.pMesh->SetPos(x, y, z);
	newPL.intensity = 1.0f;
	m_PLights.push_back(std::move(newPL));

	m_selPLight = index;
	return index;
}

LightMgr::PLight& LightMgr::PL() noexcept {
	if (m_selPLight < m_PLights.size()) {
		return m_PLights[m_selPLight];
	}
}

LightMgr::PLight& LightMgr::PL(std::string name) noexcept {
	
	uint16_t index = 0;
	for (auto& it : m_PLights)
	{
		if (it.name == name)
		{
			m_selPLight = index;
			return it;
		}
		index++;
	}

	name = "Point light '" + name + "' not found.";
	MessageBoxA(nullptr, name.c_str(), "LightMgr Error", MB_OK | MB_ICONWARNING);
	return m_PLights[m_selPLight];
}

LightMgr::PLight& LightMgr::PL(uint16_t index) noexcept {
	if (index < m_PLights.size())
	{
		m_selPLight = index;
		return m_PLights[index];
	}

	MessageBoxA(nullptr, "Index out of boundaries.", "LightMgr Error", MB_OK | MB_ICONWARNING);
	return m_PLights[m_selPLight];
}

bool& LightMgr::ShowPLMeshes() noexcept
{
	return MeshPointLight::m_showAllMeshes;
}