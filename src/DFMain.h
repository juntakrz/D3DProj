#pragma once

#include "DF_Includes.h"
#include "imgui/imgui.h"

class DFMain
{
	using json = nlohmann::json;

	float m_SimSpeed = 1.0f;
	float deltaA = 0.0f;

	std::string mapName = "";										// current map

	void LoadJSON(const wchar_t* path, json& out_j) noexcept;		// loads JSON file into a provided object
	void LoadMaterialsFromJSON(const json& materials) noexcept;		// parses JSON file and loads materials from it
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