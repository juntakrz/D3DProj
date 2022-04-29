#pragma once

#include "../../pch.h"
#include "DFSurface.h"
#include "Util/DF_Data.h"

class DFMaterial
{
	friend class DFModelMgr;

	const std::string constPath = "textures//";

	struct Material
	{
		std::string name;

		std::string shaderVertex, shaderPixel;
		uint16_t idTex[6] = { 0, 0, 0, 0, 0, 0 };

		XMFLOAT4 ambientColor;
		XMFLOAT3A F0;
		
		/* Material Data Structure
		* 
		x = material intensity
		y = specular intensity	/ metalness (PBS)
		z = specular power		/ roughness (PBS)
		*/
		XMFLOAT4 data;

		// will MatMgr automatically try to delete textures
		// from memory if unused by any other material
		bool manageTextures = false;
	};

	struct DFTexture
	{
		std::string name;
		std::string filePath;
		std::shared_ptr<ID3D11ShaderResourceView*> pSRV;
	};

	std::vector<std::unique_ptr<Material>> m_Materials;
	std::vector<DFTexture> m_DFTextures;

public:

	struct DFMATERIAL_DESC
	{
		std::string name;
		bool manageTextures = false;

		struct
		{
			std::string vertex = "VS_Default", pixel = "PS_Default";
		} shaders;

		struct
		{
			std::string tex0 = "default//default.dds", tex1 = "", tex2 = "", tex3 = "", tex4 = "", tex5 = "";
		} textures;

		struct
		{
			XMFLOAT4 ambientColor = { 0.0f, 0.0f, 0.0f, 0.0f };
			XMFLOAT3A F0 = { 0.4f, 0.4f, 0.4f };		//basic metal
			float matIntensity = 1.0f;
			float spec_metal = 1.0f;
			float pow_roughness = 1.0f;
			float extra = 0.0f;
		} material;
	};

	DFMaterial() {};
	~DFMaterial() = default;
	DFMaterial(const DFMaterial&) = delete;

	static DFMaterial& Get() noexcept {
		static DFMaterial _SInstance;
		return _SInstance;
	}

	uint16_t MatAdd(DFMATERIAL_DESC* pDesc) noexcept;
	Material& Mat(std::string name) noexcept;
	Material& Mat(uint16_t index) noexcept;
	uint16_t MatIndex(std::string name) const noexcept;
	uint16_t MatCount() const noexcept;
	void MatDelete(uint16_t index) noexcept;
	void MatDelete(std::string name) noexcept;

	uint16_t TextureAdd(std::string filePath) noexcept;
	uint16_t TextureAdd(std::string name, std::string filePath) noexcept;
	ID3D11ShaderResourceView* TextureGet(uint16_t index) const noexcept;
	ID3D11ShaderResourceView* TextureGet(std::string name) const noexcept;
	std::string TextureGetName(uint16_t index) const noexcept;
	uint16_t TextureGetIndex(std::string name) const noexcept;
	bool TextureDelete(uint16_t index) noexcept;
	bool TextureDelete(std::string name) noexcept;

	void DEBUG_ShowMaterialIndex(uint16_t begin = 0, uint16_t end = 65535) const noexcept;
	void DEBUG_ShowTextureIndex(uint16_t begin = 0, uint16_t end = 65535) const noexcept;
};