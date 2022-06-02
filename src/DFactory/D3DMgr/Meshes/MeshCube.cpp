#include "MeshCube.h"

MeshCube::MeshCube(uint16_t matId, uint16_t paramA, uint16_t paramB)
{
	auto model = CCube::Create<DF::Vertex>();
	model.SetTangentBinormalNormal();

	// calculate mesh bounding box
	CalcMeshAABBPoints(model.vertices);

	//
	// CORE BUFFERS
	//

	// create VertexBuffer with vertices
	m_Binds[Bind::idVertexBuffer] = std::make_unique<Bind::VertexBuffer>(model.vertices);

	// create IndexBuffer with indices
	m_Binds[Bind::idIndexBuffer] = std::make_unique<Bind::IndexBuffer>(model.indices);

	// create topology
	m_Binds[Bind::idTopology] = std::make_unique<Bind::Topology>(D3D11_PRIMITIVE_TOPOLOGY::D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// assign core pointers
	m_pIndexBuffer = reinterpret_cast<Bind::IndexBuffer*>(m_Binds[Bind::idIndexBuffer].get());

	/*	//////////	*/

	// load color texture
	m_Binds[Bind::idTexture0] = std::make_unique<Bind::Texture>(MatMgr.TextureGet("default//default.dds"));

	m_Binds[Bind::idSampler0] = std::make_unique<Bind::Sampler>();

	// fill material const buffer
	AddMaterialBind(matId);

	// create vertex shader
	std::string VSPath = "shaders//" + MatMgr.Mat(matId).shaderVertex + ".shd";
	std::unique_ptr<Bind::VertexShader> pVS = std::make_unique<Bind::VertexShader>(VSPath);
	ID3DBlob* pVSByteCode = pVS->GetByteCode();
	m_Binds[Bind::idVertexShader] = std::move(pVS);

	//create and bind pixel shader
	std::string PSPath = "shaders//" + MatMgr.Mat(matId).shaderPixel + ".shd";
	m_Binds[Bind::idPixelShader] = std::make_unique<Bind::PixelShader>(PSPath);

	//create and bind InputLayout
	m_Binds[Bind::idInputLayout] = std::make_unique<Bind::InputLayout>(DF::D3DLayout, pVSByteCode);

	//create and bind transform constant buffer
	m_Binds[Bind::idTransform] = std::make_unique<Bind::TransformConstBuffer>(*this);
}