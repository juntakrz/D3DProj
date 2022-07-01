#pragma once

#include "../pch.h"
#include "Common/DF_A.h"
#include "Common/DF_Const.h"

class DFMaterial
{
	friend class DFModelMgr;

	const std::string constPath = "textures//";

	struct Material
	{
		uint16_t id;
		std::string name;

		std::string shaderVertex, shaderPixel, shaderGeometry;
		std::string idTex[6] = { "", "", "", "", "", ""};

		XMFLOAT4 ambientColor;
		XMFLOAT3A F0;
		
		/* Material Data Structure
		* 
		x = material intensity
		y = specular intensity	/ metalness (PBS)
		z = specular power		/ roughness (PBS)
		w = bumpmapping intensity
		*/
		XMFLOAT4 data;

		// set related render passes using bitwise ops
		uint32_t passes;

		// will MatMgr automatically try to delete textures
		// from memory if unused by any other material
		bool manageTextures = false;
	};

	struct DFTexture
	{
		std::string name;
		std::string filePath;
		std::shared_ptr<ID3D11ShaderResourceView*> pSRV = nullptr;
		ID3D11RenderTargetView* pRTV = nullptr;		//used by render to textures
		ID3D11Texture2D* pTex = nullptr;
	};

	struct DFShader
	{
		COMPTR<ID3D11VertexShader>		pVS	= nullptr;
		COMPTR<ID3D11GeometryShader>	pGS	= nullptr;
		COMPTR<ID3D11PixelShader>		pPS	= nullptr;
		COMPTR<ID3DBlob>				pSData;
	};

	std::vector<std::unique_ptr<Material>> m_Materials;
	std::unordered_map<std::string, DFTexture> m_DFTextures;
	std::unordered_map<std::string, DFShader> m_Shaders;

public:

	struct DFMATERIAL_DESC
	{
		std::string name;
		bool manageTextures = false;

		struct
		{
			std::string vertex = "VS_Default", pixel = "PS_Default", geometry = "";
		} shaders;

		struct
		{
			std::string tex0 = "default//default.dds", tex1 = "", tex2 = "", tex3 = "", tex4 = "", tex5 = "";
		} textures;

		struct
		{
			XMFLOAT4 ambientColor = { 0.0f, 0.0f, 0.0f, 0.0f };
			XMFLOAT3A F0 = { 0.4f, 0.4f, 0.4f };		//basic metal
			float intensity = 1.0f;
			float metalness = 1.0f;
			float roughness = 1.0f;
			float bumpiness = 1.0f;
		} material;

		uint32_t passes = DF::Pass::Standard;
	};

	DFMaterial() {};
	~DFMaterial() = default;
	DFMaterial(const DFMaterial&) = delete;

	static DFMaterial& Get() noexcept {
		static DFMaterial _SInstance;
		return _SInstance;
	}

	// MATERIALS

	uint16_t MatAdd(DFMATERIAL_DESC* pDesc) noexcept;

	Material& Mat(std::string name) noexcept;
	Material& Mat(uint16_t index) noexcept;
	uint16_t MatIndex(std::string name) const noexcept;
	uint16_t MatCount() const noexcept;
	void MatDelete(uint16_t index) noexcept;
	void MatDelete(std::string name) noexcept;

	// TEXTURES

	std::string TextureAdd(std::string filePath) noexcept;
	std::string TextureAdd(std::string name, std::string filePath) noexcept;
	std::string TextureAddRT(std::string name, UINT width, UINT height) noexcept;

	std::shared_ptr<ID3D11ShaderResourceView*> TextureGet(const std::string& name) const noexcept;
	bool TextureExists(const std::string& name) const noexcept;
	bool TextureIsRT(const std::string& name) const noexcept;
	bool TextureDelete(std::string name) noexcept;

	bool BindTextureToPS(const std::string& texture, const UINT& slot, const UINT& num = 1) noexcept;	// bind texture as a PS resource
	bool BindTextureToGS(const std::string& texture, const UINT& slot, const UINT& num = 1) noexcept;	// bind texture as a GS resource

	// SHADERS

	void*		ShaderAdd(const std::string& filePath) noexcept;		// it's up to a caller to reinterpret the pointer to shader
	void*		ShaderGet(const std::string& id) const noexcept;
	ID3DBlob*	ShaderByteCode(const std::string& id) const noexcept;
	bool		ShaderDelete(const std::string& id) noexcept;

	void DEBUG_ShowMaterialIndex(uint16_t begin = 0, uint16_t end = 65535) const noexcept;
	void DEBUG_ShowTextureIndex(uint32_t begin = 0, uint32_t end = 99999) const noexcept;
};