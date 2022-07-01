#pragma once

#include "DF_Includes.h"
#include "imgui/imgui.h"

class DFMain
{
	using json = nlohmann::json;

	float m_SimSpeed = 1.0f;
	float deltaA = 0.0f;

	std::string mapName = "";										// current map

	// JSON
	void JSONLoad(const wchar_t* path, json& out_j) noexcept;		// loads JSON file into a provided object
	void JSONParseCameras(const json& cameras) noexcept;			// parses JSON file with camera settings
	void JSONParseMaterials(const json& materials) noexcept;		// parses JSON file and loads materials from it
	void JSONParseLights(const json& lights) noexcept;				// parses JSON file for lights
	void JSONParseObjects(const json& objects) noexcept;			// parses JSON file with meshes to be pre-loaded
	void JSONParseCommands(const json& commands) noexcept;			// parses initial commands for preloaded objects

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