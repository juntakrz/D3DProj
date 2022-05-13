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
		// mark bits that will run this mesh through corresponding passes
		fxNone =		0,						// exclude from rendering
		fxStandard =	1 << 0,					// pass 0
		fxBlur =		1 << 1,					// pass 1
		fxMask =		1 << 2,					// pass 2
		fxOutline =		(1 << 2) + (1 << 3)		// pass 2 and 3
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