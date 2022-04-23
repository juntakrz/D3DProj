#include "LightMgr.h"

LightMgr::DirLight_ConstPSBuffer& LightMgr::DL() noexcept
{
	return dlData;
}

uint16_t LightMgr::GetMaxPLights() const noexcept
{
	return m_maxPLights;
}

void LightMgr::ShowControls() noexcept
{
	/*if (ImGui::Begin("LIGHT CONTROLS"))
	{
		ImGui::Text("Position");
		ImGui::SliderFloat("X", &data.pos.x, -400.0f, 400.0f, "%.1f");
		ImGui::SliderFloat("Y", &data.pos.y, -400.0f, 400.0f, "%.1f");
		ImGui::SliderFloat("Z", &data.pos.z, -50.0f, 400.0f, "%.1f");
		if (ImGui::Button("Reset"))
		{
			Reset();
		}
	}
	ImGui::End();*/
}

void LightMgr::Reset() noexcept {
	plData.pos[m_selPLight] = {0.0f, 0.0f, 0.0f};
}

void LightMgr::Draw(D3DMgr& d3dMgr) const noexcept {

	for (const auto& it : m_PLights) {
		it.pMesh->Draw(d3dMgr);
	}
}

void LightMgr::Bind(D3DMgr& d3dMgr, const DirectX::XMMATRIX& camView, MeshCore* mesh) noexcept {

	//this method calculates which point lights are within rendering distance of the model
	//and fills the constant PS buffer with them

	uint8_t addedPLights = 0;

	XMVECTOR lightPos;
	XMFLOAT3 distanceToCam;

	FXMVECTOR modelPos = XMLoadFloat3(mesh->GetXMPos());

	for (uint16_t i = 0; i < m_PLights.size() && addedPLights < m_maxPLights; i++) {
		
		lightPos = XMLoadFloat3(m_PLights[i].pMesh->GetXMPos());
		XMStoreFloat3(&distanceToCam, XMVector3Length(lightPos - modelPos));
		if (distanceToCam.x < m_PLights[i].intensity * 16.0f)
		{
			plData.pos[addedPLights] = *m_PLights[i].pMesh->GetXMPos();
			plData.diffuse[addedPLights] = m_PLights[i].color;
			plData.intensity[addedPLights].x = m_PLights[i].intensity;
			addedPLights++;
		}
	}

	plData.numPLights.x = addedPLights;

	PointLight_ConstPSBuffer plDataCopy = plData;
	
	for (uint16_t j = 0; j < addedPLights; j++) {

		DirectX::XMStoreFloat3(&plDataCopy.pos[j], DirectX::XMVector3Transform(
			DirectX::XMLoadFloat3(&plData.pos[j]), camView));
	}
	

	pointBuffer.Update(d3dMgr, plDataCopy);
	pointBuffer.Bind(d3dMgr);

	//updating and binding directional light buffer
	dirBuffer.Update(d3dMgr, dlData);
	dirBuffer.Bind(d3dMgr);
}