#pragma once

#include "DF_Includes.h"
#include "imgui/imgui.h"

class DFMain
{
	float m_SimSpeed = 1.0f;
	float deltaA = 0.0f;

	void DrawFrame();

public:
	DFMain(DFactory::DFACTORY_INIT_DESC *pDesc);
	~DFMain() {};
	DFMain(const DFMain&) = delete;
	DFMain& operator=(const DFMain&) = delete;

	DFactory& DF;

	int32_t Run();
	void LoadScreen() noexcept;
	void CreateMaterials() noexcept;
};