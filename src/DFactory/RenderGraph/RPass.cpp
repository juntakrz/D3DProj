#include "RPass.h"
#include "../DFactory.h"

// static definition
DFactory* RPass::Engine = nullptr;

void RPass::PassJobAdd(RPassJob&& job) noexcept
{
	m_Jobs.emplace_back(std::move(job));
}

void RPass::DrawAABBs() noexcept
{
	// get pass technique
	m_pTech = &m_pTechDB->m_Techniques[m_Id];

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
		for (auto& it : m_Jobs)
		{
			// bind core mesh buffers
			it.pMesh->BindCore();
			it.pMesh->Binds()->at(Bind::idTransform)->Bind();

			// begin occlusion query
			it.pMesh->BeginQuery();

			// issue draw call
			it.pMesh->DrawIndexed();

			// end and write the result to primary mesh
			it.pMesh->EndQuery();

			it.pMesh->GetQueryResult(it.pMesh);
		}
		break;
	}
	case false:
	{
		for (auto& it : m_Jobs)
		{
			it.pMesh->m_QueryResult = 100;
		}
		break;
	}
	}

	// clear depth buffer
	//DF::D3DM->ClearDSBuffer(m_pTech->m_DSB);
}

void RPass::Draw() noexcept
{
	// get pass technique
	m_pTech = &m_pTechDB->m_Techniques[m_Id];

	// switch to pass-defined camera, if available
	// or switch to camera tied to special id, if defined
	(m_pTech->m_Camera != "")
		? (m_pTech->m_Camera == "$active_camera")
			? DF::D3DM->SetCamera(DF::Engine->Camera(DF::Engine->vars.activeCamera))
			: DF::D3DM->SetCamera(DF::Engine->Camera(m_pTech->m_Camera))
		: void();

	(m_pTech->BindMode() == RTechnique::BIND_TECHNIQUE) ? m_pTech->BindTechnique() : void();

	// switch to pass render / depth stencil buffers or leave previous ones as is, if undefined
	DF::D3DM->RTBind(m_pTech->m_RB, m_pTech->m_DSB);

	// change depth stencil state if defined
	(m_pTech->m_depthState > -1) ? DF::D3DM->SetDepthStencilState(m_pTech->m_depthState) : void();

	for (auto& it : m_Jobs)
	{
		// bind core mesh buffers
		it.pMesh->BindCore();

		// set bind mode
		switch (m_pTech->BindMode())
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
		(m_pTech->m_BindLights) ? Engine->LightM->Bind(Engine->Camera()->m_XMView, it.pMesh) : void();

		// issue draw call
		it.pMesh->DrawIndexed();
	}
}

void RPass::DrawCSM() noexcept
{
	float coef = 2.0f;		// coefficient using which orthogonal camera will be adjusted
	m_pTech = &m_pTechDB->m_Techniques[m_Id];

	DF::D3DM->SetCamera(DF::Engine->Camera(m_pTech->m_Camera));
	if (!DF::D3DM->Camera())
	{
#ifdef _DEBUG || _DFDEBUG
		OutputDebugStringA("WARNING! PassDrawCS: 'get camera' function returned nullptr!\n");
#endif
		return;
	};

	if (!m_pTech->m_IsCShadowTechnique)
	{
#ifdef _DEBUG || _DFDEBUG
		MessageBoxA(nullptr, "Technique is not marked suitable for cascade shadow mapping.", "PassDrawCS Error", MB_OK | MB_ICONERROR);
		return;
#else
		MessageBoxA(nullptr, "Render pass misconfigured.", "Renderer Error", MB_OK | MB_ICONERROR);
		return;
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
			m_pTechDB->CSM_Data.cascade0.width * powf(coef, cascade),
			m_pTechDB->CSM_Data.cascade0.height * powf(coef, cascade),
			m_pTechDB->CSM_Data.cascadeData.nearZ[cascade],
			m_pTechDB->CSM_Data.cascadeData.farZ[cascade]
		);

		// update viewport offset
		DF::Context()->RSSetViewports(1u, &m_pTechDB->CSM_Data.vp[cascade]);

		// update camera view matrix with new projection settings
		DF::D3DM->Camera()->SetViewProj();

		// store current camera view projection in dir light buffer (maybe put it into camera object?)
		DF::Engine->LightM->DLVSBufferSetViewProj(cascade);

		for (auto& it : m_Jobs)
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
}

void RPass::DrawSprites(const std::string& DS_SRV) noexcept
{
	// get technique data
	m_pTech = &m_pTechDB->m_Techniques[m_Id];

	// set appropriate render targets
	DF::D3DM->RTBind(m_pTech->m_RB, m_pTech->m_DSB);

	// disable depth buffer
	DF::D3DM->SetDepthStencilState((uint8_t)DF::DS_Mode::DepthOff);

	// switch to technique-defined camera, if available
	// or switch to camera tied to special id, if defined
	(m_pTech->m_Camera != "")
		? (m_pTech->m_Camera == "$active_camera")
		? DF::D3DM->SetCamera(DF::Engine->Camera(DF::Engine->vars.activeCamera))
		: DF::D3DM->SetCamera(DF::Engine->Camera(m_pTech->m_Camera))
		: void();

	// bind technique binds once
	m_pTech->BindTechnique();

	// retrieve sampler from the pixel shader... but need to do it in a better way
	COMPTR<ID3D11SamplerState> pSState;

	D3D11_SAMPLER_DESC samplerDesc{};
	samplerDesc.Filter = D3D11_FILTER_COMPARISON_MIN_MAG_LINEAR_MIP_POINT;
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
	samplerDesc.MinLOD = 0.0f;
	samplerDesc.MaxLOD = 0.0f;
	samplerDesc.MaxAnisotropy = 0.0f;
	samplerDesc.ComparisonFunc = D3D11_COMPARISON_LESS;

	DF::Device()->CreateSamplerState(&samplerDesc, &pSState);
	
	DF::Context()->GSSetSamplers(1u, 1u, pSState.GetAddressOf());
	DF::D3DM->Context();
	
	DF::Context()->GSSetShaderResources(0u, 1u, DF::D3DM->GetDepthTarget(DS_SRV)->pDS_SRV.GetAddressOf());

	// run sprite rendering jobs
	for (auto& it : m_Jobs)
	{
		it.pMesh->BindCore();		// overriden in MeshPoint to bind vertex buffer only
		it.pMesh->BindLocal();
		it.pMesh->DrawIndexed();	// this is a point and has no indices, so DrawIndexed() is overriden with Draw()
	}

	// clear geometry shader
	DF::Context()->GSSetShader(nullptr, nullptr, NULL);

	// enable depth buffer
	DF::D3DM->SetDepthStencilState((uint8_t)DF::DS_Mode::Default);
}
