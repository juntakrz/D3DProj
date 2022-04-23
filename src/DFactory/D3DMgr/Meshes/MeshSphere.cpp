#include "MeshSphere.h"

MeshSphere::MeshSphere(D3DMgr& d3dMgr, std::string material, uint16_t divisions, bool invertFaces)
{
	m_pD3DMgr = &d3dMgr;
	m_matIndex = MatMgr.MatIndex(material);
	
	if (!IsStaticBindsInitialized())
	{
		//create and bind topology
		AddStaticBind(std::make_unique<Bind::Topology>(d3dMgr, D3D11_PRIMITIVE_TOPOLOGY::D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST));
	}
	else
	{
		//get index buffer for all the following insantces of this object
		SetIndexBufferFromStaticBinds();
	}

	//create instance
	auto model = CSphere::Create<Vertex>(divisions, invertFaces);
	model.SetTangentBinormalNormal();

	//create and bind VertexBuffer with vertices
	AddBind(std::make_unique<Bind::VertexBuffer>(d3dMgr, model.vertices), Bind::idVertexBuffer);

	//create and bind IndexBuffer with indices
	AddIndexBuffer(std::make_unique<Bind::IndexBuffer>(d3dMgr, model.indices));

	//load color texture
	AddBind(std::make_unique<Bind::Texture>(d3dMgr, *MatMgr.Mat(material).pTexBase), Bind::idTexture0);

	//load normal texture (if exists)
	const auto texNormal = MatMgr.Mat(material).pTexNormal;
	if (texNormal != nullptr) {
		AddBind(std::make_unique<Bind::Texture>(d3dMgr, *MatMgr.Mat(material).pTexNormal, 1u), Bind::idTexture1);
	}
	AddBind(std::make_unique<Bind::Sampler>(d3dMgr), Bind::idSampler);

	//fill material const buffer
	AddMaterialBind(m_matIndex);

	//create and bind vertex shader
	std::string VSPath = "shaders//" + MatMgr.Mat(m_matIndex).shaderVertex + ".cso";
	std::unique_ptr<Bind::VertexShader> pVS = std::make_unique<Bind::VertexShader>(d3dMgr, VSPath);
	ID3DBlob* pVSByteCode = pVS->GetByteCode();
	AddBind(std::move(pVS), Bind::idVertexShader);

	//create and bind pixel shader
	std::string PSPath = "shaders//" + MatMgr.Mat(m_matIndex).shaderPixel + ".cso";
	AddBind(std::make_unique<Bind::PixelShader>(d3dMgr, PSPath), Bind::idPixelShader);

	//create and bind InputLayout
	std::vector<D3D11_INPUT_ELEMENT_DESC> ied =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "BINORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};
	AddBind(std::make_unique<Bind::InputLayout>(d3dMgr, ied, pVSByteCode), Bind::idInputLayout);

	//create and bind transform constant buffer
	AddBind(std::make_unique<Bind::TransformConstBuffer>(d3dMgr, *this), Bind::idTransform);
}

DirectX::XMMATRIX MeshSphere::GetTransformXM() const noexcept
{
	DirectX::XMStoreFloat3A(&xmPos, DirectX::FXMVECTOR(xmMain.r[3]));

	return xmMain;
}