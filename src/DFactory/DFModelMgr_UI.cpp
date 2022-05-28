#include "DFModelMgr.h"

void DFModelMgr::ShowEditWindow() noexcept
{
	auto& model = m_Models[m_curModel];
	bool modelAdjusted = false;

	auto check = [&](const bool& result)
	{
		(!modelAdjusted && result) ? modelAdjusted = result : 0;
	};

	if (!DFModelM_UIData.storedT)
	{
		DFModelM_UIData.tempTranslation = model.transform.translation;
		DFModelM_UIData.storedT = true;
	}

	if (!DFModelM_UIData.storedR)
	{
		DFModelM_UIData.tempRotation = model.transform.rotation;
		DFModelM_UIData.storedR = true;
	}

	if (!DFModelM_UIData.storedS)
	{
		DFModelM_UIData.tempScaling = model.transform.scaling;
		DFModelM_UIData.storedS = true;
	}

	ImGui::PushFont(DF::Engine->m_imFont);

	if (ImGui::Begin("EDIT MODEL", 0, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse))
	{
		// set window
		const auto& res = pD3DMgr->GetResolution();
		ImGui::SetWindowPos(ImVec2(res.x - 328.0f, 10.0f));
		ImGui::SetWindowSize(ImVec2(320.0f, 500.0f));
		ImGui::PushItemWidth(ImGui::GetWindowWidth() * 0.9f);

		// basic stats
		ImGui::Text("name: %s (id: %d)\n\n", model.name.c_str(), model.id);

		// translation
		ImGui::PushID("idT");
		ImGui::Text("Translation:\n");
		ImGui::Text("X");
		ImGui::SameLine();
		check(ImGui::DragFloat("##X", &model.transform.translation.x, 0.01f, -100.0f, 100.0f, "%.2f", 1.0f));
		ImGui::Text("Y");
		ImGui::SameLine();
		check(ImGui::DragFloat("##Y", &model.transform.translation.y, 0.01f, -100.0f, 100.0f, "%.2f", 1.0f));
		ImGui::Text("Z");
		ImGui::SameLine();
		check(ImGui::DragFloat("##Z", &model.transform.translation.z, 0.01f, -100.0f, 100.0f, "%.2f", 1.0f));

		if (modelAdjusted)
			std::string x;

		if (ImGui::Button("Recall", ImVec2(96, 20)))
		{
			model.transform.translation = DFModelM_UIData.tempTranslation;
			modelAdjusted = true;
		}
		ImGui::SameLine();
		if (ImGui::Button("Save", ImVec2(96, 20)))
		{
			DFModelM_UIData.storedT = false;
		}

		ImGui::PopID();

		// rotation
		ImGui::PushID("idR");
		ImGui::Text("\nRotation:\n");
		ImGui::Text("X");
		ImGui::SameLine();
		ImGui::SliderFloat("##X", &model.transform.rotation.x, -XM_PI, XM_PI, "%.2f", 1.0f);
		ImGui::Text("Y");
		ImGui::SameLine();
		ImGui::SliderFloat("##Y", &model.transform.rotation.y, -XM_PI, XM_PI, "%.2f", 1.0f);
		ImGui::Text("Z");
		ImGui::SameLine();
		ImGui::SliderFloat("##Z", &model.transform.rotation.z, -XM_PI, XM_PI, "%.2f", 1.0f);

		if (ImGui::Button("Recall", ImVec2(96, 20)))
		{
			model.transform.rotation = DFModelM_UIData.tempRotation;
			modelAdjusted = true;
		}
		ImGui::SameLine();
		if (ImGui::Button("Save", ImVec2(96, 20)))
		{
			DFModelM_UIData.storedR = false;
		}
		ImGui::SameLine();
		if (ImGui::Button("Default", ImVec2(96, 20)))
		{
			model.transform.rotation = { 0.0f, 0.0f, 0.0f };
			modelAdjusted = true;
		}

		ImGui::PopID();

		// scaling
		ImGui::PushID("idS");
		ImGui::NewLine();
		ImGui::Text("Scaling:");
		ImGui::SameLine(ImGui::GetWindowWidth() - 80.0f);
		ImGui::Text("Uniform");
		ImGui::SameLine();
		ImGui::Checkbox("##uniform", &DFModelM_UIData.CBX_UniformS);
		switch (DFModelM_UIData.CBX_UniformS)
		{
		case true:
		{
			ImGui::PushItemWidth(ImGui::GetWindowWidth() * 0.95f);
			check(ImGui::DragFloat("##scale", &model.transform.scaling.x, 0.001f, 0.001f, 10000.0f, "%.3f", 1.0f));
			model.transform.scaling.y = model.transform.scaling.x;
			model.transform.scaling.z = model.transform.scaling.x;
			ImGui::PushItemWidth(ImGui::GetWindowWidth() * 0.9f);
			break;
		}
		case false:
		{
			ImGui::Text("X");
			ImGui::SameLine();
			check(ImGui::DragFloat("##X", &model.transform.scaling.x, 0.001f, 0.001f, 10000.0f, "%.3f", 1.0f));
			ImGui::Text("Y");
			ImGui::SameLine();
			check(ImGui::DragFloat("##Y", &model.transform.scaling.y, 0.001f, 0.001f, 10000.0f, "%.3f", 1.0f));
			ImGui::Text("Z");
			ImGui::SameLine();
			check(ImGui::DragFloat("##Z", &model.transform.scaling.z, 0.001f, 0.001f, 10000.0f, "%.3f", 1.0f));
			break;
		}
		}

		if (ImGui::Button("Recall", ImVec2(96, 20)))
		{
			model.transform.scaling = DFModelM_UIData.tempScaling;
			modelAdjusted = true;
		}
		ImGui::SameLine();
		if (ImGui::Button("Save", ImVec2(96, 20)))
		{
			DFModelM_UIData.storedS = false;
		}
		ImGui::SameLine();
		if (ImGui::Button("Default", ImVec2(96, 20)))
		{
			model.transform.scaling = { 0.1f, 0.1f, 0.1f };
			modelAdjusted = true;
		}

		ImGui::PopID();
	}
	ImGui::End();
	ImGui::PopFont();
}