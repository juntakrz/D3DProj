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

	// check if current technique requires mesh binds and if yes - bind it only once per pass
	const bool& requiresMeshBinds = m_pTech->RequiresMeshBinds();
	(requiresMeshBinds) ? void() : m_pTech->BindTechnique();

	// switch to pass camera, if available
	(m_pTech->m_pCamera) ? DF::D3DM->SetCamera(m_pTech->m_pCamera) : void();

	// switch to pass render / depth stencil buffers or leave previous ones as is, if undefined
	(m_pTech->m_RB > -1 && m_pTech->m_DSB > -1) ? DF::D3DM->RTBind(m_pTech->m_RB, m_pTech->m_DSB) : void();

	// change depth stencil state if defined
	(m_pTech->m_depthState > -1) ? DF::D3DM->SetDepthStencilState(m_pTech->m_depthState) : void();

	for (auto& it : m_Jobs)
	{
		// bind core mesh buffers
		it.pMesh->BindCore();

		// bind mesh own transform buffer for passes not requiring mesh binds
		// or bind mesh own bindables if requires
		(requiresMeshBinds) ?
			it.pMesh->BindLocal() : it.pMesh->Binds()->at(Bind::idTransform)->Bind();

		// need to bind light pos const buffer here (but only for standard passes)
		(this->m_Id < 2) ? Engine->LightM->Bind(Engine->Camera()->m_XMView, it.pMesh) : void();

		// issue draw call
		it.pMesh->DrawIndexed();
	}
}
