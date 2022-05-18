#include "RPass.h"
#include "../DFactory.h"

// static definition
DFactory* RPass::Engine = nullptr;

void RPass::PassJobAdd(RPassJob&& job) noexcept
{
	m_Jobs.emplace_back(std::move(job));
}

void RPass::PassDraw() const noexcept
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

	// if pass is 0 or previous pass camera differs from the current one - recalculate matrices
	if (m_Id == 0 || (m_Id > 0 && m_pTechDB->m_Techniques[m_Id - 1].m_Camera != m_pTech->m_Camera))
	{
		DF::D3DM->Camera()->SetView();
		DF::D3DM->Camera()->SetViewProj();
	}

	(m_pTech->BindMode() == RTechnique::BIND_TECHNIQUE) ? m_pTech->BindTechnique() : void();

	// switch to pass render / depth stencil buffers or leave previous ones as is, if undefined
	(m_pTech->m_RB > -1 && m_pTech->m_DSB > -1) ? DF::D3DM->RTBind(m_pTech->m_RB, m_pTech->m_DSB) : void();

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
		(this->m_Id < 2) ? Engine->LightM->Bind(Engine->Camera()->m_XMView, it.pMesh) : void();

		// issue draw call
		it.pMesh->DrawIndexed();
	}
}
