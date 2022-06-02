#pragma once

#include "../D3DMgr/Meshes/MeshCore.h"
#include "RTechnique.h"

struct RPassJob
{
	MeshCore* pMesh;
	MeshCore* pOTMesh = nullptr;
};

class RPass
{
	friend class RenderGraph;
	static class DFactory* Engine;

	static void Init() noexcept
	{
		RPass::Engine = reinterpret_cast<DFactory*>(DF::Engine);
	};

public:
	RPass(size_t id, std::string name) noexcept : m_Id(id), m_Name(name), m_pTechDB(&RTechniqueDB::Get())
	{
	};
	RPass(const RPass&) = default;
	~RPass() = default;

	// add rendering job
	void PassJobAdd(RPassJob&& job) noexcept;

	// execute jobs

	// occlusion query pass, tests meshes or AABBs against meshes to perform/skip draw calls
	void DrawAABBs() noexcept;

	// generic draw pass
	void Draw() noexcept;

	// cascade shadow mapping pass, relies on an appropriate technique
	void DrawCSM() noexcept;

	// special sprite layer that uses GS to expand points to quads, optionally requires depth buffer
	void DrawSprites(const std::string& DS_SRV) noexcept;
	
private:
	size_t m_Id;						// pass Id, which should be the same for technique steps
	std::string m_Name;					// pass name to identify it
	std::vector<RPassJob> m_Jobs;		// pass will apply corresponding technique step to mesh
	
	RTechniqueDB* m_pTechDB = nullptr;			// technique database
	mutable RTechnique* m_pTech = nullptr;		// only used inside PassDraw for faster calls
};