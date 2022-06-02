#pragma once

#include "RPass.h"
#include "../DFactory.h"
#include "../Util/GPUConstBuffer.h"

// RENDER QUEUE MANAGER

class RenderGraph
{
public:
	// when initialized - pre-generates default techniques
	RenderGraph() noexcept;
	RenderGraph(const RenderGraph&) = default;
	~RenderGraph() = default;

	void Render() noexcept;						// frame processing scenario

	void PassCreate(std::string name) noexcept;	// create pass
	RPass& Pass(std::string name) noexcept;		// get pass

	void ResetRenderPasses() noexcept;			// clears all render jobs
	void GenerateJob(MeshCore* pMesh, uint32_t techniqueIds) noexcept;
	void GenerateQueryJob(MeshCore* pMesh, MeshCore* pOTMesh) noexcept;

private:
	// store passes and techniques
	std::vector<RPass> m_Passes;
	std::unordered_map<std::string, CBuffer> m_globalCBuffers;
};