#include "RenderGraph.h"

RenderGraph::RenderGraph() noexcept
{
	// create passes that correspond to their technique Id
	PassCreate("CSM");				// 0 = special CSM pass
	PassCreate("Query");			// 1 = occlusion query pass
	PassCreate("Background");		// 2 = fxBackground
	PassCreate("Standard0");		// 3 = fxStandard
	PassCreate("Blur");				// 4 = fxBlur
	PassCreate("Foreground");		// 5 = experimental occlusion query pass
	PassCreate("AABBShow");			// 6 = fxAABBShow
	PassCreate("Outline0");			// 7 = fxOutline (step 0)
	PassCreate("Outline1");			// 8 = fxOutline (step 1)

	// initialize default techniques
	RTechniqueDB::Get().InitDefaultTechniques();

	// initialize static pointers to engine methods in Pass class
	RPass::Init();
}

void RenderGraph::Render() noexcept
{
	// RENDER SCENARIO (need to rewrite this)
	
	//
	// PASS PROCESSOR
	//

	m_Passes[0].DrawCSM();
	//m_Passes[0].DrawAABBs();
	m_Passes[2].Draw();					// fxBackground
	m_Passes[3].Draw();					// fxStandard
	m_Passes[4].Draw();					// fxBlur - draw to 'blur' buffer and 'render' depth pass

	// create a texture out of depth buffer by copying it to a free resource
	DF::D3DM->RTCopyBuffer(1u, 4u, true);

	m_Passes[1].DrawAABBs();			// draw AABBs to depth buffer twice and query occlusion of AABBs in depth buffer
	//m_Passes[5].DrawOT();				// fxForeground - NOT READY YET
	//m_Passes[6].Draw();				// fxAABBShow
	m_Passes[7].Draw();					// fxOutline stencil writing step
	m_Passes[8].Draw();					// fxOutline stencil masking step

	// disable stencil
	DF::D3DM->SetDepthStencilState((uint8_t)DF::DS_Mode::Default);
	
	//
	// SURFACE PROCESSOR
	//

	// bind 'downsample' buffer and depth buffer with equal resolution
	DF::D3DM->RTBind(DF::RBuffers::Resample, DF::DSBuffers::Resample);

	// render to downscaled surface that has blur shaders using fxBlur buffer
	DF::D3DM->RenderBufferToSurface(1u, DF::RBuffers::Blur);

	// copy "render" depth buffer to its compatible clone if visualizing depth
	//(DF::D3DM->GetShowDepth()) ? DF::D3DM->RTCopyBuffer(1u, 4u, true) : void();

	// bind 'render' buffer and corresponding depth buffer
	DF::D3DM->RTBind(DF::RBuffers::Render, DF::DSBuffers::Render);

	DF::D3DM->RenderBufferToSurface(1u, DF::RBuffers::Resample);

	// bind main back buffer and its depth buffer
	DF::D3DM->RTBind(DF::RBuffers::Back, DF::DSBuffers::Back);

	// draw the primary surface using data from render buffer 1 or draw depth data
	(DF::D3DM->GetShowDepth())
		? DF::D3DM->RenderDepthToSurface(0u, (DF::DSBuffers)DF::DEBUG.depthView)
		: DF::D3DM->RenderBufferToSurface(0u, DF::RBuffers::Render);
}

void RenderGraph::PassCreate(std::string name) noexcept
{
	for (const auto& it : m_Passes)
	{
		if (it.m_Name == name)
		{
#ifdef _DEBUG || _DFDEBUG
			name = "RenderGraph Error: Pass '" + name + "' already exists.";
			OutputDebugStringA(name.c_str());
#endif
			return;
		};
	}
	m_Passes.emplace_back(RPass{ m_Passes.size(), name });
}

RPass& RenderGraph::Pass(std::string name) noexcept
{
	for (auto& it : m_Passes)
	{
		if (it.m_Name == name)
		{
			return it;
		}
	}

#ifdef _DEBUG || _DFDEBUG
	name = "RenderGraph: Pass " + name + " not found. Returning Pass 0.\n";
	OutputDebugStringA(name.c_str());
#endif

	return m_Passes[0];
}

void RenderGraph::ResetRenderPasses() noexcept
{
	for (auto& it : m_Passes)
	{
		it.m_Jobs.clear();
	}
}

void RenderGraph::GenerateJob(MeshCore* pMesh, uint32_t techniqueIds) noexcept
{
	if (!techniqueIds)	// if 0 - ignore and don't render
	{
		return;
	}
	
	for (uint8_t i = 0; i < RTechniqueDB::Get().Size(); i++)
	{
		// add render job at pass equal to technique index
		// job: a ptr to mesh, binds will be received from technique with the same index as this pass
		(techniqueIds >> i & 1) ? m_Passes[i].PassJobAdd(RPassJob{ pMesh }) : void();
	}
}

void RenderGraph::GenerateQueryJob(MeshCore* pMesh, MeshCore* pOTMesh) noexcept
{
	// will use 'OTMesh' for querying / occlusion testing and will write the result to 'Mesh'
	m_Passes[1].PassJobAdd(RPassJob{ pMesh, pOTMesh });
}
