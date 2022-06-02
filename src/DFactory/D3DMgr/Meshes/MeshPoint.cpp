#include "MeshPoint.h"

MeshPoint::MeshPoint(uint16_t matId) noexcept
{
	std::vector<DF::Vertex> vertices;
	DF::Vertex v;
	v.pos = { 0.0f, 0.0f, 0.0f };
	vertices.emplace_back(v);

	std::string VS = "VS_PtSprite";
	std::string PS = "PS_PtSprite";
	std::string GS = "GS_PtSprite";

	// create vertex buffer
	m_Binds[Bind::idVertexBuffer] = std::make_unique<Bind::VertexBuffer>(vertices);

	// load color texture
	m_Binds[Bind::idTexture0] = std::make_unique<Bind::Texture>(MatMgr.TextureGet("default//default.dds"));

	m_Binds[Bind::idSampler0] = std::make_unique<Bind::Sampler>();

	// fill material const buffer
	AddMaterialBind(matId);

	// create vertex shader
	std::string VSPath = "shaders//" + VS + ".shd";
	std::unique_ptr<Bind::VertexShader> pVS = std::make_unique<Bind::VertexShader>(VSPath);
	ID3DBlob* pVSByteCode = pVS->GetByteCode();
	m_Binds[Bind::idVertexShader] = std::move(pVS);

	std::string GSPath = "shaders//" + GS + ".shd";
	m_Binds[Bind::idGeometryShader] = std::make_unique<Bind::GeometryShader>(GSPath);

	//create and bind pixel shader
	std::string PSPath = "shaders//" + PS + ".shd";
	m_Binds[Bind::idPixelShader] = std::make_unique<Bind::PixelShader>(PSPath);

	//create and bind InputLayout
	std::vector<D3D11_INPUT_ELEMENT_DESC> ied =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};

	m_Binds[Bind::idInputLayout] = std::make_unique<Bind::InputLayout>(ied, pVSByteCode);

	//create and bind transform constant buffer
	m_Binds[Bind::idTransform] = std::make_unique<Bind::TransformConstBuffer>(*this);
}

void MeshPoint::BindCore() const noexcept
{
	m_Binds[Bind::idVertexBuffer]->Bind();
}

void MeshPoint::DrawIndexed() noexcept
{
	DF::Context()->Draw(1, 0);
}
