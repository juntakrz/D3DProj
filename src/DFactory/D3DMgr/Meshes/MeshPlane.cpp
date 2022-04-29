#include "MeshPlane.h"

MeshPlane::MeshPlane(uint16_t matId, uint16_t divisionsX, uint16_t divisionsY)
{

	if (!IsStaticBindsInitialized())
	{
		//create and bind topology
		AddStaticBind(std::make_unique<Bind::Topology>(D3D11_PRIMITIVE_TOPOLOGY::D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST));
	}
	else
	{
		//get index buffer for all the following insantces of this object
		SetIndexBufferFromStaticBinds();
	}

	//create instance
	auto model = CPlane::Create<Vertex>(divisionsX, divisionsY);
	model.SetTangentBinormalNormal();

	//create and bind VertexBuffer with vertices
	AddBind(std::make_unique<Bind::VertexBuffer>(model.vertices), Bind::idVertexBuffer);

	//create and bind IndexBuffer with indices
	AddIndexBuffer(std::make_unique<Bind::IndexBuffer>(model.indices));

	//load color texture
	AddBind(std::make_unique<Bind::Texture>(MatMgr.TextureGet(0)), Bind::idTexture0);
	/*
	//load normal texture (if exists)
	const auto texNormal = MatMgr.TextureGet(MatMgr.Mat(m_matIndex).idTex1);
	if (texNormal != nullptr) {
		AddBind(std::make_unique<Bind::Texture>(MatMgr.TextureGet(MatMgr.Mat(m_matIndex).idTex1), 1u), Bind::idTexture1);
	}
	*/
	AddBind(std::make_unique<Bind::Sampler>(), Bind::idSampler);
	
	//fill material const buffer
	AddMaterialBind(matId);

	//create and bind vertex shader
	std::string VSPath = "shaders//" + MatMgr.Mat(matId).shaderVertex + ".cso";
	std::unique_ptr<Bind::VertexShader> pVS = std::make_unique<Bind::VertexShader>(VSPath);
	ID3DBlob* pVSByteCode = pVS->GetByteCode();
	AddBind(std::move(pVS), Bind::idVertexShader);

	//create and bind pixel shader
	std::string PSPath = "shaders//" + MatMgr.Mat(matId).shaderPixel + ".cso";
	AddBind(std::make_unique<Bind::PixelShader>(PSPath), Bind::idPixelShader);

	//create and bind InputLayout
	std::vector<D3D11_INPUT_ELEMENT_DESC> ied =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "BINORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};
	AddBind(std::make_unique<Bind::InputLayout>(ied, pVSByteCode), Bind::idInputLayout);

	//create and bind transform constant buffer
	AddBind(std::make_unique<Bind::TransformConstBuffer>(*this), Bind::idTransform);
}

DirectX::XMMATRIX MeshPlane::GetTransformXM() const noexcept
{
	DirectX::XMStoreFloat3A(&xmPos, DirectX::FXMVECTOR(xmMain.r[3]));

	return xmMain;
}