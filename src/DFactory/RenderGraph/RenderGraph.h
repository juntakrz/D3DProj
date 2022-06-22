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

	// stores GPU dynamic constant buffers
	std::unordered_map<std::string, CBuffer> m_DCBs;

private:
	// RENDER PASS METHODS
	bool Pass(const char* technique) noexcept;								// generic pass
	bool PassCSM(const char* technique) noexcept;							// cascade shadow mapping pass
	bool PassQuery(const char* technique) noexcept;							// occlusion query pass (uses OT meshes)
	bool PassSprites(const char* technique, const char* depthSRV) noexcept;	// sprite/billboard pass

	/* runs surface blur with one or more downsampling stages, doesn't run if stages are 0 (currently only 1 stage is implemented)
	takes a texture/buffer to blur and outputs the result to the provided texture/buffer
	depth texture is optional if it depth data needs to be replaced with a fullscreen quad */
	bool PostBlur(const char* in_RT, const char* out_RT, const char* out_DS = "", const uint8_t& stages = 1) noexcept;

	// merges depth buffers A & B into selected target and depth buffer using set surface
	void MergeDepthBuffers(const char* idA, const char* idB, const char* dsTarget, const char* surface) noexcept;

	// DYNAMIC CONSTANT BUFFER METHODS
	CBuffer& DCB(const char* id) noexcept;									// creates or returns ptr to dynamic constant buffer

public:
	// INITIALIZATION
	RenderGraph() noexcept;
	RenderGraph(const RenderGraph&) = default;
	~RenderGraph() = default;

	static RenderGraph& Get()
	{
		static RenderGraph _SInstance;
		return _SInstance;
	}

	// RENDER SCENARIO
	void RenderFrame() noexcept;										// execute frame scenario sequence

	// RENDER JOBS
	void ResetJobs() noexcept;											// clear all render jobs
	void CreateJob(MeshCore* pMesh, uint32_t techniqueIds) noexcept;	// create rendering job
	void CreateQueryJob(MeshCore* pMesh, MeshCore* pOTMesh) noexcept;	// create occlusion query job
};