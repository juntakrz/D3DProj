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
	// overload for meshes without AABBs
}

DFModelNode::DFModelNode(std::vector<MeshCore*> pMeshes, std::vector<MeshCore*> pAABBs, const XMMATRIX& transform) noexcept
	: m_pMeshes(std::move(pMeshes)), m_pAABBs(std::move(pAABBs)), m_nodeTransform(transform)
{
	// overload for node meshes with AABBs
}

void DFModelNode::XMUpdate(FXMMATRIX transform) noexcept
{
	m_accTransform = m_nodeTransform * transform;

	for (const auto& pAABB : m_pAABBs)
	{
		pAABB->XMUpdate(m_accTransform);
	}

	for (const auto& mesh : m_pMeshes)
	{
		mesh->XMUpdate(m_accTransform);
	}

	for (const auto& node : m_pChildNodes)
	{
		node->XMUpdate(m_accTransform);
	}
}

void DFModelNode::CreateRenderJob(RenderGraph* renderMgr) const noexcept
{
	for (uint32_t i = 0; i < m_pMeshes.size(); i++)
	{
		// generate job using technique flags
		(i < m_pAABBs.size() && m_pAABBs[i])
			? (m_pMeshes[i]->m_queryResult > 0) ? renderMgr->CreateJob(m_pMeshes[i], m_pMeshes[i]->GetTechniqueIds()) : void()
			: renderMgr->CreateJob(m_pMeshes[i], m_pMeshes[i]->GetTechniqueIds());

		// generate occlusion test query jobs using AABBs if available
		(i < m_pAABBs.size() && m_pAABBs[i] && m_pMeshes[i]->m_distanceToCamera > DF::minQueryDepth) ? renderMgr->CreateQueryJob(m_pMeshes[i], m_pAABBs[i]) : void();
		
		// if distance from camera/view to mesh is less than query depth threshold - always draw it
		(m_pMeshes[i]->m_distanceToCamera < DF::minQueryDepth) ? m_pMeshes[i]->m_queryResult = true : 0;

		// if debug-showing AABBs enabled - show them in a special pass
		(true) ? (i < m_pAABBs.size() && m_pAABBs[i]) ? renderMgr->CreateJob(m_pAABBs[i], DF::Pass::AABBShow) : void() : void();
	}

	/*
	for (const auto& mesh : m_pMeshes)
	{
		renderMgr->GenerateJob(mesh, mesh->GetTechniqueIds());
	}

	// show AABBs in a special dedicated pass when enabled
	for (const auto& pAABB : m_pAABBs)
	{
		(pAABB) ? renderMgr->GenerateJob(pAABB, DF::Layer::AABBShow) : void();
	}*/

	for (const auto& pNode : m_pChildNodes)
	{
		pNode->CreateRenderJob(renderMgr);
	}
}