#include "MeshPointLight.h"

bool MeshPointLight::m_showAllMeshes = false;

MeshPointLight::MeshPointLight()
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
	auto model = CSphere::Create<Vertex>(4);

	//create and bind VertexBuffer with vertices
	AddBind(std::make_unique<Bind::VertexBuffer>(model.vertices), Bind::idVertexBuffer);

	//create and bind IndexBuffer with indices
	AddIndexBuffer(std::make_unique<Bind::IndexBuffer>(model.indices));

	//create and bind vertex shader
	std::string VSPath = "shaders//VS_Default.cso";
	std::unique_ptr<Bind::VertexShader> pVS = std::make_unique<Bind::VertexShader>(VSPath);
	ID3DBlob* pVSByteCode = pVS->GetByteCode();
	AddBind(std::move(pVS), Bind::idVertexShader);

	//create and bind pixel shader
	std::string PSPath = "shaders//PS_Default.cso";
	AddBind(std::make_unique<Bind::PixelShader>(PSPath), Bind::idPixelShader);

	//create and bind InputLayout
	std::vector<D3D11_INPUT_ELEMENT_DESC> ied =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};
	AddBind(std::make_unique<Bind::InputLayout>(ied, pVSByteCode), Bind::idInputLayout);

	//create and bind transform constant buffer
	AddBind(std::make_unique<Bind::TransformConstBuffer>(*this), Bind::idTransform);
}

void MeshPointLight::EnableLightMesh() noexcept
{
	m_showMesh = true;
}

void MeshPointLight::DisableLightMesh() noexcept
{
	m_showMesh = false;
}

void MeshPointLight::Draw() noexcept
{
	for (const auto& it : *GetBinds())
	{
		it ? it->Bind() : void();
	}
	for (const auto& it : GetStaticBinds())
	{
		it->Bind();
	}
	(m_showMesh && m_showAllMeshes) ? DFData::pD3DM->DrawIndexed(GetIndexBuffer()->GetCount()) : (void)0;
}

void MeshPointLight::DEBUG_Rotate(float delta) noexcept
{
	mod.rotation.z += delta;
}

DirectX::XMMATRIX MeshPointLight::GetTransformXM() const noexcept
{
	//mod.rotation.z = GMath::WrapAngle(mod.rotation.z + -0.005f);

	xmMain = DirectX::XMMatrixScaling(0.02f, 0.02f, 0.02f)
		//* DirectX::XMMatrixTranslation(33.0f, 0.0f, 0.0f)
		* DirectX::XMMatrixRotationRollPitchYaw(
			GMath::WrapAngle(mod.rotation.x), GMath::WrapAngle(mod.rotation.y), GMath::WrapAngle(mod.rotation.z))
		* DirectX::XMMatrixRotationRollPitchYaw(
			GMath::WrapAngle(transform.rotation.x), GMath::WrapAngle(transform.rotation.y), GMath::WrapAngle(transform.rotation.z))
		* DirectX::XMMatrixTranslation(transform.translation.x, transform.translation.y, transform.translation.z);

	DirectX::XMStoreFloat3A(&xmPos, DirectX::FXMVECTOR(xmMain.r[3]));

	return xmMain;
}
