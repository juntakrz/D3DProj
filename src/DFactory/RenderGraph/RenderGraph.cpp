#include "RenderGraph.h"

RenderGraph::RenderGraph() noexcept
{
	// initialize default techniques
	techMgr = &RTechnique::Get();
	techMgr->InitTechniques();

	// prepare jobs based on available techniques
	for (const auto& it : techMgr->m_TechDB)
	{
		m_PassJobs[it.first] = {};
	};

	// create two const buffers to control gaussian blur passes (stores x, y - direction of sampling)
	m_globalCBuffers["gcbScanH"] = CBuffer{};
	m_globalCBuffers["gcbScanV"] = CBuffer{};
	m_globalCBuffers["gcbMainMixBlur"] = CBuffer{};
	m_globalCBuffers["gcbMainMixLens"] = CBuffer{};
	m_globalCBuffers.at("gcbScanH").GenerateBuffer(1.0f, 0.0f);
	m_globalCBuffers.at("gcbScanV").GenerateBuffer(0.0f, 1.0f);
	m_globalCBuffers.at("gcbMainMixBlur").GenerateBuffer(0.85f);		// 0.85
	m_globalCBuffers.at("gcbMainMixLens").GenerateBuffer(0.71f);
}

void RenderGraph::RenderFrame() noexcept
{
	// RENDER SCENARIO (need to rewrite this)
	
	//
	// PASS PROCESSOR
	//
	PassCSM("Shadow");					// cascade shadow mapping pass
	Pass("Background");					// depth-independent background pass
	Pass("Standard");					// standard pass
	Pass("Blur");						// draw to 'rtBlur' and 'dsMain'

	// create a texture out of depth buffer by copying it to a free resource
	DF::D3DM->RTCopyTarget("dsMain", "dsMainCopy", true);

	PassQuery("Occlusion");				// draw occluders to depth buffer (twice) and query occlusion in depth buffer
	//m_Passes[6].Draw();				// fxAABBShow
	//m_Passes[7].Draw();				// fxOutline stencil writing step
	//m_Passes[8].Draw();				// fxOutline stencil masking step

	// disable stencil
	DF::D3DM->SetDepthStencilState((uint8_t)DF::DS_Mode::Default);
	
	//
	// SURFACE PROCESSOR
	//

	// BLUR PASS #1 (downsample 2x)

	// bind 'downsample' render and depth buffers with equal resolution
	DF::D3DM->RTBind("rtDSample2", "dsDSample2");

	//DF::D3DM->RTSetAsShaderResource("dsMainCopy", DF::ShaderType::PS, 1u);

	// set horizontal direction for gauss blur
	m_globalCBuffers.at("gcbScanH").BindToPS(0u);

	// render to downscaled 2x surface that has blur shaders using fxBlur buffer
	DF::D3DM->RenderBufferToSurface("rtBlur", "sfcBlur");

	// make a copy of a buffer to render it again
	DF::D3DM->RTCopyTarget("rtDSample2", "rtDS2Copy", false);

	// clear buffers so rendering of alpha won't be additive
	DF::D3DM->Clear("rtDSample2", "dsDSample2");

	// set vertical direction for gauss blur
	m_globalCBuffers.at("gcbScanV").BindToPS(0u);

	// render 'downsample' buffer to 'blur' surface
	DF::D3DM->RenderBufferToSurface("rtDS2Copy", "sfcBlur");
	
	// BLUR PASS #2 (downsample 4x)

	// same actions as above, but render to 4x downsampled buffer
	DF::D3DM->RTBind("rtDSample4", "dsDSample4");
	m_globalCBuffers.at("gcbScanH").BindToPS(0u);			//horizontal pass
	DF::D3DM->RenderBufferToSurface("rtBlur", "sfcBlur");
	DF::D3DM->RTCopyTarget("rtDSample4", "rtDS4Copy", false);
	DF::D3DM->Clear("rtDSample4", "dsDSample4");
	m_globalCBuffers.at("gcbScanV").BindToPS(0u);			// vertical pass
	DF::D3DM->RenderBufferToSurface("rtDS4Copy", "sfcBlur");
	
	// copy "render" depth buffer to its compatible clone if visualizing depth
	//(DF::D3DM->GetShowDepth()) ? DF::D3DM->RTCopyTarget(1u, 4u, true) : void();

	// bind 'render' buffer and corresponding depth buffer
	DF::D3DM->RTBind("rtMain", "dsMain");

	// bind 4x downsampled buffer to PS so it will be mixed with 2x downscaled buffer
	DF::D3DM->RTSetAsShaderResource("rtDSample4", DF::ShaderType::PS, 1u);

	// change blur buffer mix level in a shader
	m_globalCBuffers.at("gcbMainMixBlur").BindToPS(0u);

	// render 'downsample' buffer to 'blur' surface
	DF::D3DM->RenderBufferToSurface("rtDSample2", "sfcMain");

	// draw sprites, use depth buffer to check if these are occluded
	PassSprites("PointSprites", "dsMainCopy");

	// bind main back buffer and its depth buffer
	DF::D3DM->RTBind("rtBack", "dsBack");

	// bind lens dirt texture to PS to use in the final surface rendering
	DF::Engine->MatM->BindTextureToPS("Lens//lensDust.dds", 1u);

	m_globalCBuffers.at("gcbMainMixLens").BindToPS(0u);

	//DF::D3DM->Surface("sfcMain")->SetShaders("surface//VS_Surface", "surface//PS_Surface_HiPass");

	// draw the primary surface using data from render buffer 1 or draw depth data
	(DF::D3DM->GetShowDepth())
		? DF::D3DM->RenderDepthToSurface(DF::DEBUG.depthView, "sfcDepth")
		: DF::D3DM->RenderBufferToSurface("rtMain", "sfcMain");

	DF::D3DM->Surface("sfcMain")->SetShaders("surface//VS_Surface", "surface//PS_Surface_LensEffect");
}

void RenderGraph::ResetJobs() noexcept
{
	for (auto& it : m_PassJobs)
	{
		it.second.clear();
	}
}

void RenderGraph::CreateJob(MeshCore* pMesh, uint32_t techniqueIds) noexcept
{
	if (!techniqueIds)	// if 0 - ignore and don't render
	{
		return;
	}

	auto size = m_PassJobs.size();
	
	for (uint8_t i = 0; i < size; i++)
	{
		// add render job at pass equal to technique index
		// job: a ptr to mesh, binds will be received from technique with the same index as this pass
		if (techniqueIds >> i & 1) m_PassJobs.at(DF::Pass::IdToString(Util::FlagPow(i))).emplace_back(RenderJob{ pMesh });
	}
}

void RenderGraph::CreateQueryJob(MeshCore* pMesh, MeshCore* pOTMesh) noexcept
{
	// will use 'OTMesh' for querying / occlusion testing and will write the result to 'Mesh'
	m_PassJobs.at("Occlusion").emplace_back(RenderJob{ pMesh, pOTMesh });
}
