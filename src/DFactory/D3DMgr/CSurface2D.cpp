#include "../../pch.h"
#include "CSurface2D.h"
#include "D3DMgr_Def.h"

void CSurface2D::Create() noexcept
{
	// create mesh
	vertices.push_back({ -1.0,  1.0, 0.0, 0.0  });	// top left
	vertices.push_back({  1.0,  1.0, 1.0, 0.0  });	// top right
	vertices.push_back({ -1.0, -1.0, 0.0, 1.0  });	// bottom right
	vertices.push_back({  1.0, -1.0, 1.0, 1.0  });	// bottom left

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

	// create vertex shader
	std::wstring filePath = L"shaders//" + std::wstring(m_VSFilePath.begin(), m_VSFilePath.end()) + L".shd";
	D3DReadFileToBlob(filePath.c_str(), &m_pVSBlob);
	DF::Device()->CreateVertexShader(
		m_pVSBlob->GetBufferPointer(), m_pVSBlob->GetBufferSize(), nullptr, &m_pVS
	);
	
	// create pixel shader
	filePath = L"shaders//" + std::wstring(m_PSFilePath.begin(), m_PSFilePath.end()) + L".shd";
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
}

void CSurface2D::Bind(ID3D11ShaderResourceView* pSRV) noexcept
{
	// store ptr to texture
	m_pSRV = pSRV;

	// bind resources
	DF::Context()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
	DF::Context()->IASetVertexBuffers(0u, 1u, m_pVertexBuffer.GetAddressOf(), &m_VBufferStride, &offset);
	DF::Context()->IASetInputLayout(m_pLayout.Get());
	DF::Context()->VSSetShader(m_pVS.Get(), nullptr, 0u);
	DF::Context()->PSSetShader(m_pPS.Get(), nullptr, 0u);

	// bind texture to render from
	DF::Context()->PSSetShaderResources(0u, 1u, &pSRV);
}

void CSurface2D::Unbind() noexcept
{
	DF::Context()->PSSetShaderResources(0u, 1u, &nullSRV);
}

void CSurface2D::Draw() noexcept
{
	//DF::Context()->DrawIndexed(6u, 0u, 0u);
	DF::Context()->Draw(4u, 0u);
}
