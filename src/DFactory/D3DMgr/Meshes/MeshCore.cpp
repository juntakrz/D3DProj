#include "MeshCore.h"

DFMaterial& MeshCore::MatMgr = DFMaterial::Get();

//
// BIND AND DRAW CALLS
//

uint32_t MeshCore::GetTechniqueIds() const noexcept
{
	return m_TechniqueIds;
}

void MeshCore::BindCore() const noexcept
{
	m_Binds[Bind::idVertexBuffer]->Bind();
	m_Binds[Bind::idIndexBuffer]->Bind();
	m_Binds[Bind::idTopology]->Bind();
}

void MeshCore::BindLocal() const noexcept
{
	for (uint8_t i = 3; i < Bind::MAXBINDS; i++)
	{
		m_Binds[i] ? m_Binds[i]->Bind() : void();
	}

	m_Binds[Bind::idTransform] ? m_Binds[Bind::idTransform]->Bind() : void();
}

std::vector<std::unique_ptr<Bind::IBind>>* MeshCore::Binds() noexcept
{
	return &m_Binds;
}

const Bind::IndexBuffer* MeshCore::GetIndexBuffer() const noexcept
{
	return m_pIndexBuffer;
}

void MeshCore::DrawIndexed() noexcept
{
	DF::D3DM->DrawIndexed(m_pIndexBuffer->GetCount());
}
/*	//////////	*/

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

void MeshCore::SetMaterial(uint16_t matIndex) noexcept
{
	// index of a material that will be used by the standard technique
	m_MaterialIndex = matIndex;
}

void MeshCore::SetMaterial(std::string name) noexcept
{
	// get material object
	const auto& mat = MatMgr.Mat(name);

	// get techniques
	m_TechniqueIds = mat.effects;

	// prepare material PS const buffer
	matCBuffer.ambientColor = mat.ambientColor;
	matCBuffer.data = mat.data;
	matCBuffer.F0 = mat.F0;

	// update binds
	for (uint8_t i = 0; i < sizeof(mat.idTex) / sizeof(uint32_t); i++)
	{
		m_Binds[Bind::idTexture0 + i] =
			std::make_unique<Bind::Texture>(MatMgr.TextureGet(mat.idTex[i]), i);
	}

	auto pVS = std::make_unique<Bind::VertexShader>("shaders//" + mat.shaderVertex + ".shd");
	ID3DBlob* pVSByteCode = pVS->GetByteCode();
	m_Binds[Bind::idVertexShader] = std::move(pVS);
	m_Binds[Bind::idPixelShader] = std::make_unique<Bind::PixelShader>("shaders//" + mat.shaderPixel + ".shd");

	m_Binds[Bind::idConstPixelBuf0] = std::make_unique<Bind::ConstPixelBuffer<MaterialPSConstBuffer>>(matCBuffer, 0u);

	m_Binds[Bind::idInputLayout] = std::make_unique<Bind::InputLayout>(DF::D3DLayout, pVSByteCode);
}

void MeshCore::SetMaterialRT(std::string name) noexcept
{
	// mark it as a render target mesh
	isRenderTarget = true;
	
	// get material object
	const auto& mat = MatMgr.Mat(name);

	//get buffer copy resource
	m_Binds[Bind::idTexture0] = std::make_unique<Bind::Texture>(
		std::make_shared<ID3D11ShaderResourceView*>(DF::D3DM->m_pBufCopy_SR.Get()), 0
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

	m_Binds[Bind::idInputLayout] = std::make_unique<Bind::InputLayout>(DF::D3DLayout, pVSByteCode);
}

void MeshCore::SetShaders(std::string& inVS, std::string& inPS) noexcept
{
	// change shaders for the current material
	auto pVS = std::make_unique<Bind::VertexShader>("shaders//" + inVS + ".shd");
	ID3DBlob* pVSByteCode = pVS->GetByteCode();
	m_Binds[Bind::idVertexShader] = std::move(pVS);
	m_Binds[Bind::idPixelShader] = std::make_unique<Bind::PixelShader>("shaders//" + inPS + ".shd");

	m_Binds[Bind::idInputLayout] = std::make_unique<Bind::InputLayout>(DF::D3DLayout, pVSByteCode);
}

void MeshCore::SetEffects(uint32_t techniqueIds) noexcept
{
	m_TechniqueIds = techniqueIds;
}