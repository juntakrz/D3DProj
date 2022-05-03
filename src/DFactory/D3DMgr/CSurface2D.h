#pragma once

//#include "../Common/DF_A.h"

using namespace DirectX;

// forward declaration
namespace DF
{
	ID3D11Device* Device() noexcept;
	ID3D11DeviceContext* Context() noexcept;
}

// 2D surface used for post processing
class CSurface2D
{
	// surface mesh data
	std::vector<XMFLOAT4> vertices;	// 0 = x, 1 = y, 2 = u, 3 = v
	std::vector<UINT> indices;

	// texture for surface
	ID3D11ShaderResourceView* m_pSRV = nullptr;
	ID3D11ShaderResourceView* nullSRV = nullptr;

	// Direct3D bindables
	COMPTR<ID3D11Buffer>		m_pVertexBuffer;
	COMPTR<ID3D11VertexShader>	m_pVS;
	COMPTR<ID3D11PixelShader>	m_pPS;
	COMPTR<ID3DBlob>			m_pVSBlob;
	COMPTR<ID3DBlob>			m_pPSBlob;
	COMPTR<ID3D11InputLayout>	m_pLayout;

	const UINT m_VBufferStride = sizeof(XMFLOAT4);
	const UINT m_IBufferStride = sizeof(UINT);
	const UINT offset = 0u;

	std::string m_PSFilePath = "PS_FSPP";
	//std::string m_PSFilePath = "PS_FSPP_Blur32";
	std::string m_VSFilePath = "VS_FSPP";

public:
	CSurface2D() {};
	CSurface2D(const CSurface2D&) = default;
	~CSurface2D() = default;

	void Create() noexcept;
	void Bind(ID3D11ShaderResourceView* pSRV) noexcept;
	void Unbind() noexcept;
	void Draw() noexcept;
};