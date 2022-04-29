#include "MeshCube.h"

MeshCube::MeshCube(uint16_t matId, uint16_t paramA, uint16_t paramB)
{

	if (!IsStaticBindsInitialized())
	{
		auto model = CCube::Create<DF::Vertex>();
		model.SetTangentBinormalNormal();

		//create and bind VertexBuffer with vertices
		AddStaticBind(std::make_unique<Bind::VertexBuffer>(model.vertices));

		//create and bind IndexBuffer with indices
		AddStaticIndexBuffer(std::make_unique<Bind::IndexBuffer>(model.indices));

		//create and bind topology
		AddStaticBind(std::make_unique<Bind::Topology>(D3D11_PRIMITIVE_TOPOLOGY::D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST));
	}
	else
	{
		//get index buffer for all the following insantces of this object
		SetIndexBufferFromStaticBinds();
	}

	//load color texture
	AddBind(std::make_unique<Bind::Texture>(MatMgr.TextureGet(0)), Bind::idTexture0);

	AddBind(std::make_unique<Bind::Sampler>(), Bind::idSampler);

	//fill material const buffer
	AddMaterialBind(matId);

	//create and bind vertex shader
	std::string VSPath = "shaders//" + MatMgr.Mat(matId).shaderVertex + ".shd";
	std::unique_ptr<Bind::VertexShader> pVS = std::make_unique<Bind::VertexShader>(VSPath);
	ID3DBlob* pVSByteCode = pVS->GetByteCode();
	AddBind(std::move(pVS), Bind::idVertexShader);

	//create and bind pixel shader
	std::string PSPath = "shaders//" + MatMgr.Mat(matId).shaderPixel + ".shd";
	AddBind(std::make_unique<Bind::PixelShader>(PSPath), Bind::idPixelShader);

	//create and bind InputLayout
	AddBind(std::make_unique<Bind::InputLayout>(DF::D3DLayout, pVSByteCode), Bind::idInputLayout);

	//create and bind transform constant buffer
	AddBind(std::make_unique<Bind::TransformConstBuffer>(*this), Bind::idTransform);
}

DirectX::XMMATRIX MeshCube::GetTransformXM() const noexcept
{
	DirectX::XMStoreFloat3A(&xmPos, DirectX::FXMVECTOR(xmMain.r[3]));

	return xmMain;
}