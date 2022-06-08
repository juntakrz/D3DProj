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

	// generate gauss blur kernel buffer
	std::vector<float> coef = GMath::GaussBlurKernel(8, 3.0f);
	if (coef.size() < 40)	// padding
	{
		uint8_t n = 40 - coef.size();
		for (uint8_t i = 0; i < n; i++)
		{
			coef.emplace_back(0.0f);
		}
	}

	// create two const buffers to control gaussian blur passes (stores x, y - direction of sampling)
	DCB("ScanH").GenerateBuffer(1.0f, 0.0f);
	DCB("ScanV").GenerateBuffer(0.0f, 1.0f);

	DCB("GaussCoef").GenerateBuffer(std::move(coef));
	DCB("MainMixBlur").GenerateBuffer(1.00f);		// 0.88
	DCB("MainMixLens").GenerateBuffer(0.51f);		// 0.71
	DCB("MainMixBloom").GenerateBuffer(0.91f);
}

void RenderGraph::RenderFrame() noexcept
{
	// RENDER SCENARIO (need to rewrite this)
	
	//
	// PASS PROCESSOR
	//

	// LAYER: SHADOW
	PassCSM("Shadow");					// cascade shadow mapping pass
	//

	// LAYER: MAIN
	Pass("Background");					// depth-off background pass
	Pass("Standard");					// standard pass
	//

	// LAYER: BLUR
	Pass("Blur");						// draw to 'rtBlur' and 'dsBlur'
	// copy to an extra depth texture as 'dsMain' will get refilled with combined depth data of 'Main' and 'Blur' layers
	DF::D3DM->RTCopyTarget("dsMain", "dsMainCopy", true);
	PostBlur("rtBlur", "rtMix");		// post process blur with input / output stages
	//

	// store layer-merged depth buffer in an extra texture for later re-use
	DF::D3DM->RTCopyTarget("dsMain", "dsMainCopy", true);

	PassQuery("Occlusion");				// draw occluders to depth buffer (twice) and query occlusion in depth buffer
	//m_Passes[6].Draw();				// fxAABBShow
	//m_Passes[7].Draw();				// fxOutline stencil writing step
	//m_Passes[8].Draw();				// fxOutline stencil masking step

	// disable stencil
	DF::D3DM->SetDepthStencilState(DF::DS_Mode::Default);
	
	// LAYER: SPRITES
	// draw sprites, use the provided depth buffer to check if these are occluded
	DF::D3DM->RTCopyTarget("rtMix", "rtMixCopy", false);
	PassSprites("PointSprites", "dsMainCopy");

	// bind main back buffer and its depth buffer
	DF::D3DM->RTBind("rtBack", "dsBack");

	// bind lens dirt texture to PS to use in the final surface rendering
	DF::Engine->MatM->BindTextureToPS("Lens/lensDust.dds", 1u);

	DCB("MainMixLens").BindToPS(0u);
	DF::D3DM->Surface("sfcMain")->SetShaders("surface/VS_Surface", "surface/PS_Surface_LensEffect");

	// draw the primary surface using data from render buffer 1 or draw depth data
	(DF::D3DM->GetShowDepth())
		? DF::D3DM->RenderDepthToSurface(DF::DEBUG.depthView, "sfcDepth")
		: DF::D3DM->RenderBufferToSurface("rtMain", "sfcMain");

	DF::D3DM->Surface("sfcMain")->SetShaders("surface/VS_Surface", "surface/PS_Surface");
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

CBuffer& RenderGraph::DCB(const char* id) noexcept
{
	switch (m_DCBs.try_emplace(id).second)
	{
	case true:
	{
		m_DCBs.at(id) = CBuffer{};
		return m_DCBs.at(id);
	}
	case false:
	{
		return m_DCBs.at(id);
	}
	}
}