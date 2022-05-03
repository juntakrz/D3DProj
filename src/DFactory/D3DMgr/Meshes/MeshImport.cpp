#include "MeshImport.h"

MeshImport::MeshImport(std::vector<std::unique_ptr<Bind::IBind>> pBinds)
{
	m_Binds = std::move(pBinds);

	m_pIndexBuffer = reinterpret_cast<Bind::IndexBuffer*>(m_Binds[Bind::idIndexBuffer].get());

	m_Binds[Bind::idTopology] = std::make_unique<Bind::Topology>(D3D11_PRIMITIVE_TOPOLOGY::D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// create transform buffer
	m_Binds[Bind::idTransform] = std::make_unique<Bind::TransformConstBuffer>(*this);

	m_TechniqueIds = DF::fxStandard;
}