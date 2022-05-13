#pragma once

#include "../D3DMgr/Meshes/MeshCore.h"
#include "RTechnique.h"

struct RPassJob
{
	MeshCore* pMesh;
};

class RPass
{
	friend class RenderQ;
	static class DFactory* Engine;

	static void Init() noexcept
	{
		RPass::Engine = reinterpret_cast<DFactory*>(DF::Engine);
	};

public:
	RPass(size_t id, std::string name) noexcept : m_Id(id), m_Name(name), m_pTechDB(&RTechniqueDB::Get()) {};
	RPass(const RPass&) = default;
	~RPass() = default;

	// add rendering job
	void PassJobAdd(RPassJob&& job) noexcept;

	// execute jobs
	void PassDraw() const noexcept;
	
private:
	size_t m_Id;						// pass Id, which should be the same for technique steps
	std::string m_Name;					// pass name to identify it
	std::vector<RPassJob> m_Jobs;		// pass will apply corresponding technique step to mesh
	
	RTechniqueDB* m_pTechDB = nullptr;			// technique database
	mutable RTechnique* m_pTech = nullptr;		// only used inside PassDraw for faster calls
};