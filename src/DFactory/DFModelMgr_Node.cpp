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

void DFModelNode::Draw(D3DMgr& d3dMgr) const noexcept
{
	for (const auto& mesh : m_pMeshes)
	{
		mesh->Draw(d3dMgr);
	}

	for (const auto& pNode : m_pChildNodes)
	{
		pNode->Draw(d3dMgr);
	}
}