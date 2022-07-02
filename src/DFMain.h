#pragma once

#include "DF_Includes.h"
#include "DFactory/ScriptMgr/CScriptMgr.h"
#include "imgui/imgui.h"

class DFMain
{
	float m_SimSpeed = 1.0f;
	float deltaA = 0.0f;

	std::string mapName = "";										// current map

	// COMMON
	void DrawFrame();												// generic frame drawing method

public:
	DFMain(DFactory::DFACTORY_INIT_DESC *pDesc);
	~DFMain() {};
	DFMain(const DFMain&) = delete;
	DFMain& operator=(const DFMain&) = delete;

	DFactory& DF;

	int32_t Run();
	void LoadMap() noexcept; // obsolete, unused, will be removed after implementing JSON
	void LoadMap(const std::wstring& map) noexcept;					// loads the map with a provided name
};