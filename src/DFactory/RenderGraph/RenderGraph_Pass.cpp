#include "RenderGraph.h"

bool RenderGraph::Pass(const char* technique) noexcept
{
	// get pass technique
	auto m_pTech = &techMgr->m_TechDB.at(technique);

	// switch to pass-defined camera, if available
	// or switch to camera tied to special id, if defined
	(m_pTech->m_Camera != "")
		? (m_pTech->m_Camera == "$active_camera")
		? DF::D3DM->SetCamera(DF::Engine->Camera(DF::Engine->vars.activeCamera))
		: DF::D3DM->SetCamera(DF::Engine->Camera(m_pTech->m_Camera))
		: void();

	(m_pTech->m_BindMode == RTechnique::BIND_TECHNIQUE) ? m_pTech->BindTechnique() : void();

	// switch to pass render / depth stencil buffers or leave previous ones as is, if undefined
	DF::D3DM->RTBind(m_pTech->m_RB, m_pTech->m_DSB);

	// change depth stencil state if defined
	(m_pTech->m_depthState > -1) ? DF::D3DM->SetDepthStencilState(m_pTech->m_depthState) : void();

	auto pJobs = &m_PassJobs.at(technique);

	for (auto& it : *pJobs)
	{
		/*
		if (it.pMesh->m_isPredicateEnabled)
		{
			DF::Context()->SetPredication(it.pMesh->m_pPredicate.Get(), FALSE);
		}
		*/

		// bind core mesh buffers
		it.pMesh->BindCore();

		// set bind mode
		switch (m_pTech->m_BindMode)
		{
		case(RTechnique::BIND_MESH):				// use only mesh binds
		{
			it.pMesh->BindLocal();
			break;
		}
		case(RTechnique::BIND_TECHNIQUE):			// use only technique binds but use mesh transform
		{
			it.pMesh->Binds()->at(Bind::idTransform)->Bind();
			break;
		}
		case(RTechnique::BIND_MESH_AND_TECHNIQUE):	// use both mesh and technique binds with latter overwriting
		{
			it.pMesh->BindLocal();
			m_pTech->BindTechnique();
			break;
		}
		}

		// need to bind light pos const buffer here (but only for standard passes)
		(m_pTech->m_BindLights) ? DF::Engine->LightM->Bind(DF::Engine->Camera()->m_XMView, it.pMesh) : void();

		// issue draw call
		it.pMesh->DrawIndexed();
		/*
		if (it.pMesh->m_isPredicateEnabled)
		{
			DF::Context()->SetPredication(NULL, FALSE);
		}*/
	}

	return true;
}

bool RenderGraph::PassCSM(const char* technique) noexcept
{
	float coef = 2.0f;		// coefficient using which orthogonal camera will be adjusted
	auto m_pTech = &techMgr->m_TechDB.at(technique);

	DF::D3DM->SetCamera(DF::Engine->Camera(m_pTech->m_Camera));
	if (!DF::D3DM->Camera())
	{
#ifdef _DEBUG || _DFDEBUG
		OutputDebugStringA("WARNING! PassDrawCS: 'get camera' function returned nullptr!\n");
#endif
		return false;
	};

	if (!m_pTech->m_IsCSM)
	{
#ifdef _DEBUG || _DFDEBUG
		MessageBoxA(nullptr, "Technique is not marked suitable for cascade shadow mapping.", "PassDrawCS Error", MB_OK | MB_ICONERROR);
		return false;
#else
		MessageBoxA(nullptr, "Render pass misconfigured.", "Renderer Error", MB_OK | MB_ICONERROR);
		return false;
#endif
	}

	// clear CSM dir light / camera buffer
	//DF::Engine->LightM->DLVSBufferClear();

	// cascade shadow mapping pass always uses technique binds
	m_pTech->BindTechnique();

	// switch to appropriate pass render / depth stencil buffers
	DF::D3DM->RTBind(m_pTech->m_RB, m_pTech->m_DSB);

	// change depth stencil state if defined
	(m_pTech->m_depthState > -1) ? DF::D3DM->SetDepthStencilState(m_pTech->m_depthState) : void();

	for (uint8_t cascade = 0; cascade < DF::CSM::cascades; cascade++)
	{
		DF::D3DM->Camera()->SetAsOrthographic(
			techMgr->CSM_Data.cascade0.width * powf(coef, cascade),
			techMgr->CSM_Data.cascade0.height * powf(coef, cascade),
			techMgr->CSM_Data.cascadeData.nearZ[cascade],
			techMgr->CSM_Data.cascadeData.farZ[cascade]
		);

		// update viewport offset
		DF::Context()->RSSetViewports(1u, &techMgr->CSM_Data.vp[cascade]);

		// update camera view matrix with new projection settings
		DF::D3DM->Camera()->SetViewProj();

		// store current camera view projection in dir light buffer (maybe put it into camera object?)
		DF::Engine->LightM->DLVSBufferSetViewProj(cascade);

		auto pJobs = &m_PassJobs.at(technique);

		for (auto& it : *pJobs)
		{
			// bind core mesh buffers
			it.pMesh->BindCore();
			it.pMesh->Binds()->at(Bind::idTransform)->Bind();

			// issue draw call
			it.pMesh->DrawIndexed();
		}
	}

	// bind CSM view proj vertex buffer
	DF::Engine->LightM->DLVSBufferBind();

	return true;
}

bool RenderGraph::PassQuery(const char* technique) noexcept
{
	// get pass technique
	auto m_pTech = &techMgr->m_TechDB.at(technique);

	// switch to pass-defined camera, if available
	// or switch to camera tied to special id, if defined
	(m_pTech->m_Camera != "")
		? (m_pTech->m_Camera == "$active_camera")
		? DF::D3DM->SetCamera(DF::Engine->Camera(DF::Engine->vars.activeCamera))
		: DF::D3DM->SetCamera(DF::Engine->Camera(m_pTech->m_Camera))
		: void();

	m_pTech->BindTechnique();

	// switch to appropriate pass render / depth stencil buffers
	DF::D3DM->RTBind(m_pTech->m_RB, m_pTech->m_DSB);

	// change depth stencil state if defined
	(m_pTech->m_depthState > -1) ? DF::D3DM->SetDepthStencilState(m_pTech->m_depthState) : void();

	// render all AABBs to depth buffer and get query results
	switch (DF::isCullingEnabled)
	{
	case true:
	{
		auto pJobs = &m_PassJobs.at(technique);

		for (auto& it : *pJobs)
		{
			// bind core mesh buffers
			it.pMesh->BindCore();
			it.pMesh->Binds()->at(Bind::idTransform)->Bind();

			// begin occlusion query
			it.pMesh->BeginQuery();
			//DF::Context()->Begin(it.pMesh->m_pPredicate.Get());

			// issue draw call
			it.pMesh->DrawIndexed();

			// end and write the result to primary mesh
			it.pMesh->EndQuery();
			//DF::Context()->End(it.pMesh->m_pPredicate.Get());

			it.pMesh->GetQueryResult(it.pMesh);
		}
		break;
	}
	case false:
	{
		auto pJobs = &m_PassJobs.at(technique);

		for (auto& it : *pJobs)
		{
			it.pMesh->m_queryResult = 100;
		}
		break;
	}
	}

	return true;
}

bool RenderGraph::PassSprites(const char* technique, const char* depthSRV) noexcept
{
	// get technique data
	auto m_pTech = &techMgr->m_TechDB.at(technique);

	// clear render target
	DF::D3DM->Clear(m_pTech->m_RB, m_pTech->m_DSB);

	// set appropriate render targets
	DF::D3DM->RTBind(m_pTech->m_RB, m_pTech->m_DSB);

	// disable depth buffer
	DF::D3DM->SetDepthStencilState((uint8_t)DF::DS_Mode::DOff_SOff);

	// switch to technique-defined camera, if available
	// or switch to camera tied to special id, if defined
	(m_pTech->m_Camera != "")
		? (m_pTech->m_Camera == "$active_camera")
		? DF::D3DM->SetCamera(DF::Engine->Camera(DF::Engine->vars.activeCamera))
		: DF::D3DM->SetCamera(DF::Engine->Camera(m_pTech->m_Camera))
		: void();

	// bind technique binds once
	m_pTech->BindTechnique();

	DF::Context()->GSSetShaderResources(0u, 1u, DF::D3DM->GetDepthTarget(depthSRV)->pDepthSRV.GetAddressOf());

	// run sprite rendering jobs
	auto pJobs = &m_PassJobs.at(technique);

	for (auto& it : *pJobs)
	{
		it.pMesh->BindCore();		// overriden in MeshPoint to bind vertex buffer only
		it.pMesh->BindLocal();
		it.pMesh->DrawIndexed();	// this is a point and has no indices, so DrawIndexed() is overriden with Draw()
	}

	// clear geometry shader
	DF::Context()->GSSetShader(nullptr, nullptr, NULL);

	// enable depth buffer
	DF::D3DM->SetDepthStencilState((uint8_t)DF::DS_Mode::Default);

	// combine rtMix with sprites and rtMixCopy with all previous layers
	DF::D3DM->RTBind("rtMain", "dsMain");
	DF::D3DM->RTSetAsShaderResource("rtMixCopy", DF::ShaderType::PS, 0u);
	DF::D3DM->RTSetAsShaderResource("rtMix", DF::ShaderType::PS, 1u);
	DF::D3DM->Surface("sfcMix")->SetShaders("surface/VS_Surface", "surface/PS_Surface_MixAdditive");
	DF::D3DM->RenderSurface("sfcMix");

	return true;
}

bool RenderGraph::PostBlur(const char* in_RT) noexcept
{
	// bind gauss kernel and coefficients buffer to PS register 2
	DCB("GaussCoef").BindToPS(2u);

	DF::D3DM->Clear("rtPPBlur", "", true);

	// bind 'downsample' render and depth buffers with equal resolution
	DF::D3DM->RTBind("rtPPBlur", "");

	// set horizontal direction for gauss blur
	DCB("ScanH").BindToPS(0u);

	// render to downscaled 2x surface that has blur shaders using fxBlur buffer
	DF::D3DM->RenderBufferToSurface(in_RT, "sfcBlur");

	// make a copy of a buffer to render it again
	DF::D3DM->RTCopyTarget("rtPPBlur", "rtPPBlurCopy", false);

	// clear buffers so rendering of alpha won't be additive
	DF::D3DM->Clear("rtPPBlur", "", true);

	// set vertical direction for gauss blur
	DCB("ScanV").BindToPS(0u);

	// render 'downsample' buffer to 'blur' surface
	DF::D3DM->RenderBufferToSurface("rtPPBlurCopy", "sfcBlur");

	return true;
}

bool RenderGraph::PostBloom(const char* in_RT) noexcept
{
	// prepare render target
	DF::D3DM->Clear("rtPPBloom");
	DF::D3DM->RTBind("rtPPBloom", "");

	DCB("stepBloom").BindToPS(1u);
	DF::D3DM->RenderBufferToSurface(in_RT, "sfcBloom");

	PostBlur("rtPPBloom");

	DF::D3DM->RTCopyTarget("rtPPBlur", "rtPPStore2X", false);

	DCB("stepBlur").BindToPS(1u);
	PostBlur("rtPPStore2X");
	//DF::D3DM->RTCopyTarget("rtPPBlur", "rtPPStore2X", false);
	//PostBlur("rtPPStore2X");

	return true;
}

void RenderGraph::MergeDepthBuffers(const char* idA, const char* idB, const char* dsTarget, const char* surface) noexcept
{
	// unbind buffers if bound
	(DF::D3DM->m_DSId == idA || DF::D3DM->m_DSId == idB) ? DF::Context()->OMSetRenderTargets(0u, nullptr, nullptr) : void();

	// bind depth buffer B as texture
	DF::D3DM->RTSetAsShaderResource(idB, DF::ShaderType::PS, 1u);

	// bind mixing target and depth
	DF::D3DM->ClearDSBuffer(dsTarget);
	DF::D3DM->RTBind("", dsTarget);

	// render depth buffer A to surface with previously set depth buffer B
	DF::D3DM->RenderDepthToSurface(idA, "sfcMix");
}
