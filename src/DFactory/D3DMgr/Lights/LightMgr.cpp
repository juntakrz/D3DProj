#include "LightMgr.h"

uint16_t LightMgr::GetMaxPLights() const noexcept
{
	return DF::maxPointLights;
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
	plData.PL[m_selPLight].pos = {0.0f, 0.0f, 0.0f};
}

void LightMgr::Draw() const noexcept {

	for (const auto& it : m_PLights) {
		it.pMesh->DrawIndexed();
	}
}

void LightMgr::Bind(const DirectX::XMMATRIX& camView, MeshCore* mesh) noexcept {

	// this method calculates which point lights are within rendering distance of the model
	// and fills the constant PS buffer with them

	uint8_t addedPLights = 0;

	XMVECTOR lightPos;
	XMFLOAT2 distanceToModel;

	FXMVECTOR modelPos = XMLoadFloat3(mesh->GetXMPos());

	for (uint16_t i = 0; i < m_PLights.size() && addedPLights < DF::maxPointLights; i++) {
		
		lightPos = XMLoadFloat3(m_PLights[i].pMesh->GetXMPos());
		XMStoreFloat2(&distanceToModel, XMVector3Length(lightPos - modelPos));
		if (distanceToModel.x < m_PLights[i].intensity * 16.0f)
		{
			plData.PL[addedPLights].pos = *m_PLights[i].pMesh->GetXMPos();
			plData.PL[addedPLights].diffuse = m_PLights[i].color;
			plData.PL[addedPLights].intensity.x = m_PLights[i].intensity;
			addedPLights++;
		}
	}

	plData.numPLights.x = addedPLights;

	PointLight_ConstPSBuffer plDataCopy = plData;
	

	pointBuffer.Update(plDataCopy);
	pointBuffer.Bind();

	// updating and binding directional light buffer
	dirBuffer.Update(dlData);
	dirBuffer.Bind();
}