#include "MeshCore.h"

DFMaterial& MeshCore::MatMgr = DFMaterial::Get();

void MeshCore::Draw(D3DMgr& d3dMgr)
{
	for (const auto& it : m_Binds)
	{
		it ? it->Bind(d3dMgr) : void();
	}
	for (const auto& it : GetStaticBinds())
	{
		it->Bind(d3dMgr);
	}
	d3dMgr.DrawIndexed(m_pCIndexBuffer->GetCount());
}

const Bind::IndexBuffer* MeshCore::GetIndexBuffer() const noexcept
{
	return m_pCIndexBuffer;
}

void MeshCore::AddBind(std::unique_ptr<Bind::IBind> bindObj, uint8_t id) noexcept
{
	//Index buffer object must be bound using AddIndexBuffer
	ASSERT(typeid(*bindObj) != typeid(Bind::IndexBuffer));
	m_Binds[id] = std::move(bindObj);
}

void MeshCore::AddIndexBuffer(std::unique_ptr<Bind::IndexBuffer> ibuf) noexcept
{
	//Index buffer should not be bound yet
	ASSERT(m_pCIndexBuffer == nullptr);

	//get a pointer required by the Draw function
	m_pCIndexBuffer = ibuf.get();
	m_Binds[Bind::idIndexBuffer] = std::move(ibuf);
}

void MeshCore::AddMaterialBind(uint16_t matIndex) noexcept
{
	//prepare material buffer
	matCBuffer.ambientColor = MatMgr.Mat(matIndex).ambientColor;
	matCBuffer.matIntensity = MatMgr.Mat(matIndex).matIntensity;
	matCBuffer.specIntensity = MatMgr.Mat(matIndex).specIntensity;
	matCBuffer.specPower = MatMgr.Mat(matIndex).specPower;

	//bind it to pixel shader cbuffer slot 0
	m_Binds[Bind::idConstPixelBuf0] =
		std::make_unique<Bind::ConstPixelBuffer<MaterialPSConstBuffer>>(*m_pD3DMgr, matCBuffer, 0u);
}

std::vector<std::unique_ptr<Bind::IBind>>* MeshCore::GetBinds() noexcept
{
	return &m_Binds;
}

void MeshCore::SetMaterial(std::string name) noexcept
{
	// get material object
	const auto& mat = MatMgr.Mat(name);
	m_matIndex = MatMgr.MatIndex(name);

	// prepare material PS const buffer
	matCBuffer.ambientColor = mat.ambientColor;
	matCBuffer.matIntensity = mat.matIntensity;
	matCBuffer.specIntensity = mat.specIntensity;
	matCBuffer.specPower = mat.specPower;

	// update binds
	if (mat.pTexBase != nullptr) {
		m_Binds[Bind::idTexture0] = std::make_unique<Bind::Texture>(*m_pD3DMgr, *mat.pTexBase);
	}

	if (mat.pTexNormal != nullptr) {
		m_Binds[Bind::idTexture1] = std::make_unique<Bind::Texture>(*m_pD3DMgr, *mat.pTexNormal, 1u);
	}

	if (mat.pTex2 != nullptr) {
		m_Binds[Bind::idTexture2] = std::make_unique<Bind::Texture>(*m_pD3DMgr, *mat.pTex2, 2u);
	}

	if (mat.pTex3 != nullptr) {
		m_Binds[Bind::idTexture3] = std::make_unique<Bind::Texture>(*m_pD3DMgr, *mat.pTex3, 3u);
	}

	if (mat.pTex4 != nullptr) {
		m_Binds[Bind::idTexture4] = std::make_unique<Bind::Texture>(*m_pD3DMgr, *mat.pTex4, 4u);
	}

	if (mat.pTex5 != nullptr) {
		m_Binds[Bind::idTexture5] = std::make_unique<Bind::Texture>(*m_pD3DMgr, *mat.pTex5, 5u);
	}

	auto pVS = std::make_unique<Bind::VertexShader>(*m_pD3DMgr, "shaders//" + mat.shaderVertex + ".cso");
	ID3DBlob* pVSByteCode = pVS->GetByteCode();
	m_Binds[Bind::idVertexShader] = std::move(pVS);
	m_Binds[Bind::idPixelShader] = std::make_unique<Bind::PixelShader>(*m_pD3DMgr, "shaders//" + mat.shaderPixel + ".cso");

	m_Binds[Bind::idConstPixelBuf0] = std::make_unique<Bind::ConstPixelBuffer<MaterialPSConstBuffer>>(*m_pD3DMgr, matCBuffer, 0u);

	std::vector<D3D11_INPUT_ELEMENT_DESC> ied =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "BINORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};
	m_Binds[Bind::idInputLayout] = std::make_unique<Bind::InputLayout>(*m_pD3DMgr, ied, pVSByteCode);
}

void MeshCore::SetShaders(std::string& inVS, std::string& inPS) noexcept
{
	// change shaders for the current material
	(inVS == "") ? inVS = MatMgr.Mat(m_matIndex).shaderVertex : MatMgr.Mat(m_matIndex).shaderVertex = inVS;
	(inPS == "") ? inPS = MatMgr.Mat(m_matIndex).shaderPixel : MatMgr.Mat(m_matIndex).shaderPixel = inPS;

	auto pVS = std::make_unique<Bind::VertexShader>(*m_pD3DMgr, "shaders//" + inVS + ".cso");
	ID3DBlob* pVSByteCode = pVS->GetByteCode();
	m_Binds[Bind::idVertexShader] = std::move(pVS);
	m_Binds[Bind::idPixelShader] = std::make_unique<Bind::PixelShader>(*m_pD3DMgr, "shaders//" + inPS + ".cso");

	std::vector<D3D11_INPUT_ELEMENT_DESC> ied =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "BINORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};
	m_Binds[Bind::idInputLayout] = std::make_unique<Bind::InputLayout>(*m_pD3DMgr, ied, pVSByteCode);
}
