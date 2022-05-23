#include "MeshPoint.h"

MeshPoint::MeshPoint(uint16_t matId, float scale) noexcept
{
	std::vector<DF::Vertex> vertices;
	DF::Vertex v;
	v.pos = { 0.0f, 0.0f, 0.0f };
	v.tex = { scale, scale };
	vertices.emplace_back(v);

	std::vector<uint32_t> indices;
	indices.emplace_back(0);

	//std::string VS = "VS_PointSprite";
	//std::string PS = "PS_PointSprite";
	std::string VS = "VS_Default";
	std::string PS = "PS_Default";
	std::string GS = "GS_PointSprite";

	// create vertex buffer
	m_Binds[Bind::idVertexBuffer] = std::make_unique<Bind::VertexBuffer>(vertices);

	// create index buffer
	m_Binds[Bind::idIndexBuffer] = std::make_unique<Bind::IndexBuffer>(indices);

	// assign core pointers
	m_pIndexBuffer = reinterpret_cast<Bind::IndexBuffer*>(m_Binds[Bind::idIndexBuffer].get());

	// create topology (it's a point)
	m_Binds[Bind::idTopology] = std::make_unique<Bind::Topology>(D3D11_PRIMITIVE_TOPOLOGY::D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);

	// load color texture
	m_Binds[Bind::idTexture0] = std::make_unique<Bind::Texture>(MatMgr.TextureGet(0));

	m_Binds[Bind::idSampler0] = std::make_unique<Bind::Sampler>();

	// fill material const buffer
	AddMaterialBind(matId);

	// create vertex shader
	std::string VSPath = "shaders//" + VS + ".shd";
	std::unique_ptr<Bind::VertexShader> pVS = std::make_unique<Bind::VertexShader>(VSPath);
	ID3DBlob* pVSByteCode = pVS->GetByteCode();
	m_Binds[Bind::idVertexShader] = std::move(pVS);

	//create and bind pixel shader
	std::string PSPath = "shaders//" + PS + ".shd";
	m_Binds[Bind::idPixelShader] = std::make_unique<Bind::PixelShader>(PSPath);

	//std::string GSPath = "shaders//" + GS + ".shd";
	//m_Binds[Bind::idGeometryShader] = std::make_unique<Bind::GeometryShader>(GSPath);

	//create and bind InputLayout
	std::vector<D3D11_INPUT_ELEMENT_DESC> ied =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};

	m_Binds[Bind::idInputLayout] = std::make_unique<Bind::InputLayout>(ied, pVSByteCode);

	//create and bind transform constant buffer
	m_Binds[Bind::idTransform] = std::make_unique<Bind::TransformConstBuffer>(*this);
}
