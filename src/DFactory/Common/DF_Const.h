#pragma once

namespace Bind
{
	constexpr uint8_t MAXBINDS = 20;

	enum BindID
	{
		idVertexBuffer, idIndexBuffer, idTopology,		// core binds
		idVertexShader, idPixelShader, idSampler,
		idTexture0, idTexture1, idTexture2, idTexture3, idTexture4, idTexture5,
		idConstVertexBuf0, idConstPixelBuf0,
		idInputLayout, idTransform
	};

	enum StencilMode
	{
		stencilOff, stencilWrite, stencilMask
	};
}

namespace DF
{
	enum ObjectID
	{
		idPlane = 0, idCube, idSphere, idSkySphere, idImport
	};

	enum FXID
	{
		// use bitwise operators for these
		fxNone =		0,			// 0
		fxStandard =	1 << 0,		// 1
		fxOutline =		1 << 1,		// 2
		fxBlur =		1 << 2,		// 4
		FX4 =			1 << 3,		// 8
		FX5 =			1 << 4,		// 16
		FX6 =			1 << 5,		// 32
		FX7 =			1 << 6,		// 64
		FX8 =			1 << 7		// 128
	};

	struct Vertex
	{
		DirectX::XMFLOAT3 pos;		//POSITION
		DirectX::XMFLOAT2 tex;		//TEXCOORD
		DirectX::XMFLOAT3 n;		//NORMAL
		DirectX::XMFLOAT3 t;		//TANGENT
		DirectX::XMFLOAT3 b;		//BINORMAL
	};

	const std::vector<D3D11_INPUT_ELEMENT_DESC> D3DLayout =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "BINORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};

}