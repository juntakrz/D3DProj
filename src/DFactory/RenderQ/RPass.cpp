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
	for (auto& it : m_Jobs)
	{
		// bind core mesh buffers
		it.pMesh->BindCore();

		// bind specific technique step buffers if present
		// otherwise bind the rest of 'standard' mesh buffers
		(it.pTStep) ? it.pTStep->StepBind() : it.pMesh->BindStandard();

		// need to bind light pos const buffer here
		Engine->LightM->Bind(Engine->Camera()->m_XMView, it.pMesh);

		// issue draw call
		it.pMesh->DrawIndexed();
	}
}
