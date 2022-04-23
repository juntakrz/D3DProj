#pragma once

//#include "../../imgui/imgui.h"

class imGuiMgr
{
public:
	imGuiMgr()
	{
		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		ImGui::StyleColorsClassic();
	}

	~imGuiMgr()
	{
		ImGui::DestroyContext();
	}

	imGuiMgr(const imGuiMgr&) = delete;
	imGuiMgr operator=(const imGuiMgr&) = delete;

	static imGuiMgr& Get()
	{
		static imGuiMgr _SInstance;
		return _SInstance;
	}

	void ImGuiBeginFrame()
	{
		ImGui_ImplDX11_NewFrame();
		ImGui_ImplWin32_NewFrame();
		ImGui::NewFrame();
	}

	void ImGuiEndFrame()
	{
		ImGui::Render();
		ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
	}
};