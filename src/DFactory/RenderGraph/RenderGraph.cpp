#include "RenderGraph.h"

RenderGraph::RenderGraph() noexcept
{
	// create passes that correspond to their technique Id
	PassCreate("CSM");				// 0 = special CSM pass
	PassCreate("Query");			// 1 = occlusion query pass
	PassCreate("Background");		// 2 = fxBackground
	PassCreate("Standard0");		// 3 = fxStandard
	PassCreate("Blur");				// 4 = fxBlur
	PassCreate("Points");			// 5 = fxPoints
	PassCreate("AABBShow");			// 6 = fxAABBShow
	PassCreate("Outline0");			// 7 = fxOutline (step 0)
	PassCreate("Outline1");			// 8 = fxOutline (step 1)

	// initialize default techniques
	RTechniqueDB::Get().InitDefaultTechniques();

	// initialize static pointers to engine methods in Pass class
	RPass::Init();

	// create two const buffers to control gaussian blur passes (stores x, y - direction of sampling)
	m_globalCBuffers["gcbScanH"] = CBuffer{};
	m_globalCBuffers["gcbScanV"] = CBuffer{};
	m_globalCBuffers.at("gcbScanH").GenerateBuffer(1.0f, 0.0f);
	m_globalCBuffers.at("gcbScanV").GenerateBuffer(0.0f, 1.0f);
}

void RenderGraph::Render() noexcept
{
	// RENDER SCENARIO (need to rewrite this)
	
	//
	// PASS PROCESSOR
	//

	m_Passes[0].DrawCSM();
	m_Passes[2].Draw();					// fxBackground
	m_Passes[3].Draw();					// fxStandard
	m_Passes[4].Draw();					// fxBlur - draw to 'blur' buffer and 'render' depth pass

	// create a texture out of depth buffer by copying it to a free resource
	DF::D3DM->RTCopyTarget("dsMain", "dsMainCopy", true);

	m_Passes[1].DrawAABBs();			// draw AABBs to depth buffer twice and query occlusion of AABBs in depth buffer
	//m_Passes[5].DrawSprites(4u);		// sprite layer, uses GS to expand points to quads
	//m_Passes[6].Draw();				// fxAABBShow
	//m_Passes[7].Draw();				// fxOutline stencil writing step
	//m_Passes[8].Draw();				// fxOutline stencil masking step

	// disable stencil
	DF::D3DM->SetDepthStencilState((uint8_t)DF::DS_Mode::Default);
	
	//
	// SURFACE PROCESSOR
	//

	// bind 'downsample' render and depth buffers with equal resolution
	DF::D3DM->RTBind("rtDSample2", "dsDSample2");

	// set horizontal direction for gauss blur
	m_globalCBuffers.at("gcbScanH").BindToPS(0u);

	// render to downscaled surface that has blur shaders using fxBlur buffer
	DF::D3DM->RenderBufferToSurface("rtBlur", "sfcBlur");

	DF::D3DM->RTCopyTarget("rtDSample2", "rtDS2Copy", false);
	DF::D3DM->Clear("rtDSample2", "dsDSample2");

	// set vertical direction for gauss blur
	m_globalCBuffers.at("gcbScanV").BindToPS(0u);

	// render 'downsample' buffer to 'blur' surface
	DF::D3DM->RenderBufferToSurface("rtDS2Copy", "sfcBlur");
	/*
	DF::D3DM->RTBind("rtDSample4", "dsDSample4");

	// set horizontal direction for gauss blur
	m_globalCBuffers.at("gcbScanH").BindToPS(0u);

	// render to downscaled surface that has blur shaders using fxBlur buffer
	DF::D3DM->RenderBufferToSurface("rtDSample2", "sfcBlur");

	DF::D3DM->RTCopyTarget("rtDSample4", "rtDS4Copy", false);
	DF::D3DM->Clear("rtDSample4", "dsDSample4");

	// set vertical direction for gauss blur
	m_globalCBuffers.at("gcbScanV").BindToPS(0u);

	// render 'downsample' buffer to 'blur' surface
	DF::D3DM->RenderBufferToSurface("rtDS4Copy", "sfcBlur");*/

	// copy "render" depth buffer to its compatible clone if visualizing depth
	//(DF::D3DM->GetShowDepth()) ? DF::D3DM->RTCopyTarget(1u, 4u, true) : void();

	// bind 'render' buffer and corresponding depth buffer
	DF::D3DM->RTBind("rtMain", "dsMain");

	// render 'downsample' buffer to 'blur' surface
	DF::D3DM->RenderBufferToSurface("rtDSample2", "sfcMain");

	// draw sprites, use depth buffer to check if these are occluded
	m_Passes[5].DrawSprites("dsMainCopy");

	// bind main back buffer and its depth buffer
	DF::D3DM->RTBind("rtBack", "dsBack");

	// bind lens dirt texture to PS to use in the final surface rendering
	DF::Engine->MatM->BindTextureToPS("Lens//lensDust.dds", 1u);

	// draw the primary surface using data from render buffer 1 or draw depth data
	(DF::D3DM->GetShowDepth())
		? DF::D3DM->RenderDepthToSurface(DF::DEBUG.depthView, "sfcDepth")
		: DF::D3DM->RenderBufferToSurface("rtMain", "sfcMain");
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

	auto size = RTechniqueDB::Get().Size();
	
	for (uint8_t i = 0; i < size; i++)
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
