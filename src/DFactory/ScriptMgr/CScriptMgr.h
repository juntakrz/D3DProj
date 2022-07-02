#pragma once

#include "../DFactory.h"

class CScriptMgr
{
private:
	using json = nlohmann::json;

	json j;
	DFactory& DF = DFactory::Get();
	std::string m_sceneName = "";

public:
	static CScriptMgr& Get() {

		static CScriptMgr _SInstance;
		return _SInstance;
	}

	// CORE
	void SetSceneName(const char* sceneName) noexcept;

	// JSON
	void JSONLoad(const wchar_t* path, json* out_j = nullptr) noexcept;	// loads JSON file into a provided object
	void JSONParseCameras(const json& cameras) noexcept;				// parses JSON file with camera settings
	void JSONParseMaterials(const json& materials) noexcept;			// parses JSON file and loads materials from it
	void JSONParseLights(const json& lights) noexcept;					// parses JSON file for lights
	void JSONParseObjects(const json& objects) noexcept;				// parses JSON file with meshes to be pre-loaded
	void JSONParseCommands(const json& commands) noexcept;				// parses initial commands for preloaded objects
};