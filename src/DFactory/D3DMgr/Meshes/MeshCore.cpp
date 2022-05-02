#include "MeshCore.h"

DFMaterial& MeshCore::MatMgr = DFMaterial::Get();

void MeshCore::Draw()
{
	for (const auto& it : m_Binds)
	{
		it ? it->Bind() : void();
	}
	for (const auto& it : GetStaticBinds())
	{
		it->Bind();
	}

	DF::pD3DM->DrawIndexed(m_pCIndexBuffer->GetCount());
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
	matCBuffer.data = MatMgr.Mat(matIndex).data;
	matCBuffer.F0 = MatMgr.Mat(matIndex).F0;

	//bind it to pixel shader cbuffer slot 0
	m_Binds[Bind::idConstPixelBuf0] =
		std::make_unique<Bind::ConstPixelBuffer<MaterialPSConstBuffer>>(matCBuffer, 0u);
}

std::vector<std::unique_ptr<Bind::IBind>>* MeshCore::GetBinds() noexcept
{
	return &m_Binds;
}

void MeshCore::SetMaterial(std::string name) noexcept
{
	// get material object
	const auto& mat = MatMgr.Mat(name);

	// prepare material PS const buffer
	matCBuffer.ambientColor = mat.ambientColor;
	matCBuffer.data = mat.data;
	matCBuffer.F0 = mat.F0;

	// update binds
	for (uint8_t i = 0; i < sizeof(mat.idTex) / sizeof(uint16_t); i++)
	{
		m_Binds[Bind::idTexture0 + i] =
			std::make_unique<Bind::Texture>(MatMgr.TextureGet(mat.idTex[i]), i);
	}

	auto pVS = std::make_unique<Bind::VertexShader>("shaders//" + mat.shaderVertex + ".shd");
	ID3DBlob* pVSByteCode = pVS->GetByteCode();
	m_Binds[Bind::idVertexShader] = std::move(pVS);
	m_Binds[Bind::idPixelShader] = std::make_unique<Bind::PixelShader>("shaders//" + mat.shaderPixel + ".shd");

	m_Binds[Bind::idConstPixelBuf0] = std::make_unique<Bind::ConstPixelBuffer<MaterialPSConstBuffer>>(matCBuffer, 0u);

	std::vector<D3D11_INPUT_ELEMENT_DESC> ied =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "BINORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};
	m_Binds[Bind::idInputLayout] = std::make_unique<Bind::InputLayout>(ied, pVSByteCode);
}

void MeshCore::SetMaterialRT(std::string name) noexcept
{
	// mark it as a render target mesh
	isRenderTarget = true;
	
	// get material object
	const auto& mat = MatMgr.Mat(name);

	//matCBuffer.ambientColor = { 0.0f, 0.0f, 0.0f, 1.0f };
	//matCBuffer.data = { 0.0f, 0.0f, 0.0f, 0.0f };
	//matCBuffer.F0 = { 0.0f, 0.0f, 0.0f };

	//get buffer copy resource
	m_Binds[Bind::idTexture0] = std::make_unique<Bind::Texture>(
		std::make_shared<ID3D11ShaderResourceView*>(DF::pD3DM->m_pSRVAux.Get()), 0
		);

	//clear unused texture binds
	for (uint8_t i = Bind::idTexture1; i < Bind::idTexture5 + 1; i++)
	{
		m_Binds[i] = nullptr;
	}

	auto pVS = std::make_unique<Bind::VertexShader>("shaders//" + mat.shaderVertex + ".shd");
	ID3DBlob* pVSByteCode = pVS->GetByteCode();
	m_Binds[Bind::idVertexShader] = std::move(pVS);
	m_Binds[Bind::idPixelShader] = std::make_unique<Bind::PixelShader>("shaders//" + mat.shaderPixel + ".shd");

	std::vector<D3D11_INPUT_ELEMENT_DESC> ied =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "BINORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};
	m_Binds[Bind::idInputLayout] = std::make_unique<Bind::InputLayout>(ied, pVSByteCode);
}

void MeshCore::SetShaders(std::string& inVS, std::string& inPS) noexcept
{
	// change shaders for the current material
	//(inVS == "") ? inVS = MatMgr.Mat(m_MatName).shaderVertex : MatMgr.Mat(m_MatName).shaderVertex = inVS;
	//(inPS == "") ? inPS = MatMgr.Mat(m_MatName).shaderPixel : MatMgr.Mat(m_MatName).shaderPixel = inPS;

	auto pVS = std::make_unique<Bind::VertexShader>("shaders//" + inVS + ".shd");
	ID3DBlob* pVSByteCode = pVS->GetByteCode();
	m_Binds[Bind::idVertexShader] = std::move(pVS);
	m_Binds[Bind::idPixelShader] = std::make_unique<Bind::PixelShader>("shaders//" + inPS + ".shd");

	std::vector<D3D11_INPUT_ELEMENT_DESC> ied =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "BINORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};
	m_Binds[Bind::idInputLayout] = std::make_unique<Bind::InputLayout>(ied, pVSByteCode);
}
