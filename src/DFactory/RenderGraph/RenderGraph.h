#pragma once

#include "../DFactory.h"
#include "RTechnique.h"
#include "../Util/GPUConstBuffer.h"
#include "../Util/Util.h"

// RENDER MANAGER

class RenderGraph
{
private:
	struct RenderJob				// render job takes mesh and optional occlusion testing mesh
	{
		MeshCore* pMesh;
		MeshCore* pOTMesh = nullptr;
	};

	// technique manager
	RTechnique* techMgr = nullptr;

	// stores render jobs, requires same key as techniques
	std::unordered_map<std::string, std::vector<RenderJob>> m_PassJobs;

	// stores dynamic GPU constant buffers
	std::unordered_map<std::string, CBuffer> m_globalCBuffers;

private:
	// RENDER PASS METHODS
	bool Pass(const char* technique) noexcept;								// generic pass
	bool PassCSM(const char* technique) noexcept;							// cascade shadow mapping pass
	bool PassQuery(const char* technique) noexcept;							// occlusion query pass (uses OT meshes)
	bool PassSprites(const char* technique, const char* depthSRV) noexcept;	// sprite/billboard pass

public:
	// INITIALIZATION
	RenderGraph() noexcept;
	RenderGraph(const RenderGraph&) = default;
	~RenderGraph() = default;

	// RENDER SCENARIO
	void RenderFrame() noexcept;										// execute frame scenario sequence

	// RENDER JOBS
	void ResetJobs() noexcept;											// clear all render jobs
	void CreateJob(MeshCore* pMesh, uint32_t techniqueIds) noexcept;	// create rendering job
	void CreateQueryJob(MeshCore* pMesh, MeshCore* pOTMesh) noexcept;	// create occlusion query job
};