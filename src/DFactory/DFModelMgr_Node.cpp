#include "DFModelMgr.h"

// NODE

void DFModelNode::AddChildNode(std::unique_ptr<DFModelNode> pChild) noexcept
{
	ASSERT(pChild);
	m_pChildNodes.push_back(std::move(pChild));
}

DFModelNode::DFModelNode(std::vector<MeshCore*> pMeshes, const XMMATRIX& transform) noexcept
	: m_pMeshes(pMeshes), m_nodeTransform(transform)
{
	//
}

void DFModelNode::XMUpdate(FXMMATRIX transform) noexcept
{
	m_accTransform = m_nodeTransform * transform;

	for (const auto& mesh : m_pMeshes)
	{
		mesh->XMUpdate(m_accTransform);
	}

	for (const auto& node : m_pChildNodes)
	{
		node->XMUpdate(m_accTransform);
	}
}

void DFModelNode::CreateRenderJob(RenderQ* renderMgr) const noexcept
{
	for (const auto& mesh : m_pMeshes)
	{
		// ask Render Manager to generate a render job using ptr to mesh and its techniques
		renderMgr->GenerateJob(mesh, mesh->GetTechniqueIds());
	}

	for (const auto& pNode : m_pChildNodes)
	{
		pNode->CreateRenderJob(renderMgr);
	}
}