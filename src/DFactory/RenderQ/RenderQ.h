#pragma once

#include "RPass.h"

// RENDER QUEUE MANAGER

class RenderQ
{
public:
	// when initialized - pre-generates default techniques
	RenderQ() noexcept;
	RenderQ(const RenderQ&) = default;
	~RenderQ() = default;

	void Render() noexcept;						// frame processing scenario

	void PassCreate(std::string name) noexcept;	// create pass
	void TechniqueCreate(uint32_t id) noexcept;	// create technique
	RPass& Pass(std::string name) noexcept;		// get pass

	void ResetRenderPasses() noexcept;			// clears all render jobs
	void GenerateJob(MeshCore* pMesh, uint32_t techniqueIds) noexcept;

private:
	// store passes and techniques
	std::vector<RPass> m_Passes;
	std::vector<std::unique_ptr<RTechnique>> m_Techniques;
};