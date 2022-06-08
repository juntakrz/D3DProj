#pragma once

using namespace DirectX;

// forward declaration
namespace DF
{
	ID3D11Device* Device() noexcept;
	ID3D11DeviceContext* Context() noexcept;
}

// 2D surface used for post processing
class CRenderSurface
{
	// surface mesh data
	std::vector<XMFLOAT4> vertices;	// 0 = x, 1 = y, 2 = u, 3 = v
	std::vector<UINT> indices;

	// texture for surface
	ID3D11ShaderResourceView* m_pSRV = nullptr;
	ID3D11ShaderResourceView* nullSRV = nullptr;

	// Direct3D bindables
	COMPTR<ID3D11Buffer>		m_pVertexBuffer;
	ID3D11VertexShader*			m_pVS;
	ID3D11PixelShader*			m_pPS;
	ID3DBlob*					m_pVSBlob;
	ID3DBlob*					m_pPSBlob;
	COMPTR<ID3D11InputLayout>	m_pLayout;
	COMPTR<ID3D11SamplerState>	m_pSampler;

	const UINT m_VBufferStride = sizeof(XMFLOAT4);
	const UINT m_IBufferStride = sizeof(UINT);
	const UINT offset = 0u;
	const float scale = 1.0f;

	std::string m_VS;
	std::string m_PS;

public:
	CRenderSurface(float scale = 1.0f, std::string VS = "surface/VS_Surface", std::string PS = "surface/PS_Surface") noexcept;
	CRenderSurface(const CRenderSurface&) = default;
	~CRenderSurface() = default;

	void SetShaders(const std::string& VS, const std::string& PS) noexcept;

	void Bind(ID3D11ShaderResourceView* pSRV = nullptr) noexcept;
	void Unbind() noexcept;
	void Draw() noexcept;
};