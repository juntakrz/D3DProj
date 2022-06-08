#include "MeshAABB.h"

MeshAABB::MeshAABB(const XMFLOAT3* AABBcoords)
{
	auto model = CCube::Create<DF::Vertex>(AABBcoords[0], AABBcoords[1]);

	// create VertexBuffer with vertices
	m_Binds[Bind::idVertexBuffer] = std::make_unique<Bind::VertexBuffer>(model.vertices);

	// create IndexBuffer with indices
	m_Binds[Bind::idIndexBuffer] = std::make_unique<Bind::IndexBuffer>(model.indices);

	// create topology
	m_Binds[Bind::idTopology] = std::make_unique<Bind::Topology>(D3D11_PRIMITIVE_TOPOLOGY::D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// assign core pointers
	m_pIndexBuffer = reinterpret_cast<Bind::IndexBuffer*>(m_Binds[Bind::idIndexBuffer].get());

	//create and bind vertex shader
	std::unique_ptr<Bind::VertexShader> pVS = std::make_unique<Bind::VertexShader>("VS_Wireframe");
	ID3DBlob* pVSByteCode = pVS->GetByteCode();
	m_Binds[Bind::idVertexShader] = std::move(pVS);

	//create and bind pixel shader
	m_Binds[Bind::idPixelShader] = std::make_unique<Bind::PixelShader>("PS_Wireframe");

	//create and bind InputLayout
	std::vector<D3D11_INPUT_ELEMENT_DESC> ied =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};
	m_Binds[Bind::idInputLayout] = std::make_unique<Bind::InputLayout>(ied, pVSByteCode);

	//create and bind transform constant buffer
	m_Binds[Bind::idTransform] = std::make_unique<Bind::TransformConstBuffer>(*this);

	// mark it for a bounding mesh usage
	m_isAABB = true;
}