#include "../../pch.h"
#include "RenderSurface.h"

RenderSurface::RenderSurface(float scale, std::string VS, std::string PS) noexcept
	: scale(scale), m_VS(VS), m_PS(PS)
{
	// create mesh
	vertices.push_back({ -scale,  scale, 0.0f, 0.0f  });	// top left
	vertices.push_back({  scale,  scale, 1.0f, 0.0f  });	// top right
	vertices.push_back({ -scale, -scale, 0.0f, 1.0f  });	// bottom right
	vertices.push_back({  scale, -scale, 1.0f, 1.0f  });	// bottom left

	// create vertex buffer
	D3D11_BUFFER_DESC vBufDesc{};
	vBufDesc.Usage = D3D11_USAGE_DEFAULT;
	vBufDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vBufDesc.ByteWidth = m_VBufferStride * 4u;
	vBufDesc.StructureByteStride = m_VBufferStride;
	vBufDesc.CPUAccessFlags = 0u;
	vBufDesc.MiscFlags = 0u;

	D3D11_SUBRESOURCE_DATA sd{};
	sd.pSysMem = vertices.data();
	
	DF::Device()->CreateBuffer(&vBufDesc, &sd, &m_pVertexBuffer);

	SetShaders(m_VS, m_PS);
}

void RenderSurface::SetShaders(const std::string& VS, const std::string& PS) noexcept
{
	m_VS = VS;
	m_PS = PS;

	// create vertex shader
	std::wstring filePath = L"shaders//" + std::wstring(VS.begin(), VS.end()) + L".shd";
	D3DReadFileToBlob(filePath.c_str(), &m_pVSBlob);
	DF::Device()->CreateVertexShader(
		m_pVSBlob->GetBufferPointer(), m_pVSBlob->GetBufferSize(), nullptr, &m_pVS
	);

	// create pixel shader
	filePath = L"shaders//" + std::wstring(PS.begin(), PS.end()) + L".shd";
	D3DReadFileToBlob(filePath.c_str(), &m_pPSBlob);
	DF::Device()->CreatePixelShader(
		m_pPSBlob->GetBufferPointer(), m_pPSBlob->GetBufferSize(), nullptr, &m_pPS
	);

	// create layout
	const std::vector<D3D11_INPUT_ELEMENT_DESC> IEDesc =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 8, D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};

	DF::Device()->CreateInputLayout(
		IEDesc.data(), IEDesc.size(), m_pVSBlob->GetBufferPointer(), m_pVSBlob->GetBufferSize(), &m_pLayout
	);

	// create sampler state
	D3D11_SAMPLER_DESC smplDesc{};
	smplDesc.AddressU = D3D11_TEXTURE_ADDRESS_MIRROR;
	smplDesc.AddressV = D3D11_TEXTURE_ADDRESS_MIRROR;
	smplDesc.AddressW = D3D11_TEXTURE_ADDRESS_MIRROR;
	smplDesc.Filter = D3D11_FILTER_ANISOTROPIC;

	DF::Device()->CreateSamplerState(&smplDesc, &m_pSampler);
}

void RenderSurface::Bind(ID3D11ShaderResourceView* pSRV) noexcept
{
	// store ptr to texture
	m_pSRV = pSRV;

	// bind resources
	DF::Context()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
	DF::Context()->IASetVertexBuffers(0u, 1u, m_pVertexBuffer.GetAddressOf(), &m_VBufferStride, &offset);
	DF::Context()->IASetInputLayout(m_pLayout.Get());
	DF::Context()->PSSetSamplers(0u, 1u, m_pSampler.GetAddressOf());
	DF::Context()->VSSetShader(m_pVS.Get(), nullptr, 0u);
	DF::Context()->GSSetShader(nullptr, nullptr, 0);
	DF::Context()->PSSetShader(m_pPS.Get(), nullptr, 0u);

	// bind texture to render from
	DF::Context()->PSSetShaderResources(0u, 1u, &pSRV);
}

void RenderSurface::Unbind() noexcept
{
	DF::Context()->PSSetShaderResources(0u, 1u, &nullSRV);
}

void RenderSurface::Draw() noexcept
{
	DF::Context()->Draw(4u, 0u);
}
