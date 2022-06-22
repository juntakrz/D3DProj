#pragma once

namespace Bind
{
	constexpr uint8_t MAXBINDS = 24;

	enum BindID
	{
		idVertexBuffer, idIndexBuffer, idTopology,		// core binds
		idVertexShader, idGeometryShader, idPixelShader,
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
	struct Pass
	{
		enum Id
		{
			// mark bits that will run this mesh through corresponding passes
			None			= 0,					// exclude from rendering
			Shadow			= 1 << 0,				// pass 0
			Occlusion		= 1 << 1,				// pass 1
			Background		= 1 << 2,				// pass 2
			Standard		= 1 << 3,				// pass 3
			Blur			= 1 << 4,				// pass 4
			PointSprites	= 1 << 5,				// pass 5
			AABBShow		= 1 << 6,				// pass 6
			Masking			= 1 << 7,				// pass 7
			Outline			= (1 << 7) + (1 << 8)	// pass 7 and 8
		};

		static std::string IdToString(const uint32_t& id) noexcept;
	};

	//
	// SHADER TYPES
	//
	struct ShaderType
	{
		enum Id
		{
			VS = 0,
			GS = 1,
			PS = 2
		};
	};

	//
	// DEPTH STENCIL DATA AND METHODS
	//
	struct DS_Mode
	{
		enum Id
		{
			Default		= 0,	// depth on	 stencil off
			DOn_SWrite	= 1,	// depth on  stencil write
			DOff_SWrite = 2,	// depth off stencil write
			DOff_SRead	= 3,	// depth off stencil read
			DOff_SOff	= 4,	// depth off stencil off
		};
	};

	struct DS_SRVMode
	{
		enum Id
		{
			None		 = 0,	// no SRV created
			Depth		 = 1,	// create SRV for depth only
			DepthStencil = 2,	// create SRV for depth and stencil
		};
	};

	struct DS_Usage
	{
		enum Id
		{
			DepthStencil = 0, DepthShadow
		};
	};

	DXGI_FORMAT GetDepthFormatTyped(const uint8_t& usage) noexcept;
	DXGI_FORMAT GetDepthFormatTypeless(const uint8_t& usage) noexcept;
	DXGI_FORMAT GetDepthFormatColor(const uint8_t& usage) noexcept;

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
	// GLOBAL VARIABLES
	//

	constexpr uint8_t maxPointLights = 8u;
	constexpr uint8_t maxTextures = 6u;
	extern bool isCullingEnabled;
	extern uint64_t framesRendered;
}