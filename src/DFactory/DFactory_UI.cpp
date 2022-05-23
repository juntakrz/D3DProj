#include "DFactory.h"

void DFactory::ShowEditModelWindow() noexcept
{
	ModelM->ShowEditWindow();
}

void DFactory::ShowCameraWindow() noexcept
{
	ImGui::PushFont(DF::Engine->m_imFont);

	if (ImGui::Begin("CAMERA METRICS", 0,
		ImGuiWindowFlags_NoResize
		| ImGuiWindowFlags_NoMove
		| ImGuiWindowFlags_NoCollapse
	))
	{
		const XMFLOAT2& res = pD3DMgr->GetResolution();
		ImGui::SetWindowPos(ImVec2(res.x - 328.0f, res.y - 248.0f));
		ImGui::SetWindowSize(ImVec2(320.0f, 200.0f));

		for (auto& it : m_Cameras)
		{
			ImGui::Text("Camera: %s\nPos: X = %.2f, Y = %.2f, Z = %.2f\n", it.first.c_str(), it.second->GetPos().x, it.second->GetPos().y, it.second->GetPos().z);
			ImGui::Text("Focus: X = %.2f, Y = %.2f, Z = %.2f\n\n", it.second->GetFocus().x, it.second->GetFocus().y, it.second->GetFocus().z);
		}
	}
	ImGui::End();
	ImGui::PopFont();
}

void DFactory::ShowStatsWindow() noexcept
{
	ImGui::PushFont(DF::Engine->m_imFont);

	if (ImGui::Begin(
		"ENGINE STATISTICS", 0,
		ImGuiWindowFlags_NoResize
		| ImGuiWindowFlags_NoMove
		| ImGuiWindowFlags_NoCollapse
		| ImGuiWindowFlags_NoDecoration
	))
	{
		const XMFLOAT2& res = pD3DMgr->GetResolution();
		ImGui::SetWindowPos(ImVec2(res.x - 328.0f, res.y - 38.0f));
		ImGui::SetWindowSize(ImVec2(320.0f, 30.0f));
		ImGui::PushItemWidth(ImGui::GetWindowWidth() * 0.8f);

		ImGui::Text("FPS: %.1f (%.1f ms) frame: %llu OT: %llu", ImGui::GetIO().Framerate, 1000.0f / ImGui::GetIO().Framerate, DF::framesRendered, debug.uint64);
		ImGui::SameLine(ImGui::GetWindowWidth() - 104.0f);
		if (ImGui::Button("QUIT", ImVec2{ 96.0f, 20.0f }))
		{
			exit(0);
		}
	}
	ImGui::End();
	ImGui::PopFont();
}