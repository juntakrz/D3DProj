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
	// check if current technique requires mesh binds and if yes - bind it only once per pass
	const bool& requiresMeshBinds = RTechniqueDB::Get().m_Techniques[m_Id].RequiresMeshBinds();
	(requiresMeshBinds) ? void() : RTechniqueDB::Get().m_Techniques[m_Id].BindTechnique();

	//if (m_Id == 2 && m_Jobs.size())
	//	std::string x;

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
