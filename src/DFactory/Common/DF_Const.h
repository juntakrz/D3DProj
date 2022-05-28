#pragma once

namespace Bind
{
	constexpr uint8_t MAXBINDS = 24;

	enum BindID
	{
		idVertexBuffer, idIndexBuffer, idTopology,		// core binds
		idVertexShader, idPixelShader, idGeometryShader,
		idSampler0, idSampler1, idRasterizer,
		idTexture0, idTexture1, idTexture2, idTexture3, idTexture4, idTexture5, idTextureDepth,
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
	//
	// POLYGONAL OBJECT ID
	//
	enum ObjectID
	{
		idPlane = 0, idCube, idSphere, idSkySphere, idPoint, idImport
	};

	//
	// RENDER TECHNIQUES
	//
	enum FXID
	{
		// mark bits that will run this mesh through corresponding passes
		fxNone			=  0,					// exclude from rendering
		fxShadow		=  1 << 0,				// pass 0
		fxOcclusion		=  1 << 1,				// pass 1
		fxBackground	=  1 << 2,				// pass 2
		fxStandard		=  1 << 3,				// pass 3
		fxBlur			=  1 << 4,				// pass 4
		fxForeground	=  1 << 5,				// pass 5
		fxAABBShow		=  1 << 6,				// pass 6
		fxMask			=  1 << 7,				// pass 7
		fxOutline		= (1 << 7) + (1 << 8)	// pass 7 and 8
	};

	//
	// BUFFERS ENUM
	//
	enum class RBuffers		// render buffer enumeration
	{
		None =		-1,
		Back =		 0,
		Render =	 1,
		Blur =		 2,
		Resample =	 3
	};

	enum class DSBuffers	// depth stencil buffer enumeration
	{
		None =		-1,
		Back =		 0,
		Render =	 1,
		Resample =	 2,
		Shadow =	 3
	};

	//
	// DEPTH STENCIL DATA AND METHODS
	//
	enum class DS_Mode
	{
		Default			= 0,
		StencilWrite	= 1,
		StencilMask		= 2,
		DepthOff		= 3,
	};

	enum class DS_Usage
	{
		DepthStencil = 0, DepthShadow
	};

	DXGI_FORMAT GetDepthFormatTyped(const DS_Usage& usage) noexcept;
	DXGI_FORMAT GetDepthFormatTypeless(const DS_Usage& usage) noexcept;
	DXGI_FORMAT GetDepthFormatColor(const DS_Usage& usage) noexcept;

	//
	// LIGHTING VARIABLES
	//
	constexpr uint8_t maxPointLights = 8u;

	//
	// CULLING VARIABLES
	//
	extern bool isCullingEnabled;

	//
	// CASCADE SHADOW MAPPING
	//
	namespace CSM
	{
		constexpr float cascades = 4.0f;	// number of cascades (must be integer)
		extern float bufferSize;			// size of a square depth texture in pixels
		extern float FOVmult;				// light camera ortho FOV multiplier
		extern float minZ;					// minimum nearZ
		extern float maxZ;					// maximum farZ
	};

	//
	// VERTEX STRUCTURE
	//

	struct Vertex
	{
		DirectX::XMFLOAT3 pos;		//POSITION
		DirectX::XMFLOAT2 tex;		//TEXCOORD
		DirectX::XMFLOAT3 n;		//NORMAL
		DirectX::XMFLOAT3 t;		//TANGENT
		DirectX::XMFLOAT3 b;		//BINORMAL
	};

	//
	//	DIRECT3D INPUT LAYOUTS
	//

	const std::vector<D3D11_INPUT_ELEMENT_DESC> D3DLayout =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "BINORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};

	//
	// METRICS
	//

	extern uint64_t framesRendered;
}