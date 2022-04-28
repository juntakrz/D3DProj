#pragma once

#include "../../pch.h"
#include "DFSurface.h"

class DFMaterial
{
	friend class DFModelMgr;

	const std::string constPath = "textures//";

	struct Material
	{
		std::string name;

		std::string shaderVertex, shaderPixel;
		DFSurface* pTexBase = nullptr;
		DFSurface* pTexNormal = nullptr;
		DFSurface* pTex2 = nullptr;
		DFSurface* pTex3 = nullptr;
		DFSurface* pTex4 = nullptr;
		DFSurface* pTex5 = nullptr;

		XMFLOAT4 ambientColor;
		XMFLOAT3A F0;
		XMFLOAT4 data;
		/* Material Data Structure
		* 
		x = material intensity
		y = specular intensity	/ metalness (PBS)
		z = specular power		/ roughness (PBS)
		*/
	};

	struct Texture
	{
		std::string name;
		std::string filePath;
		DFSurface texture;

		Texture(std::string A, std::string B, DFSurface C) : name(A), filePath(B), texture(std::move(C)) {};
	};

	std::vector<std::unique_ptr<Material>> m_Materials;
	std::vector<std::unique_ptr<Texture>> m_Textures;

public:

	struct DFMATERIAL_DESC
	{
		std::string name;

		struct
		{
			std::string vertex = "VS_Default", pixel = "PS_Default";
		} shaders;

		struct
		{
			std::string base = "default.png", normal = "", tex2 = "", tex3 = "", tex4 = "", tex5 = "";
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

	uint16_t AddTexture(std::string filePath) noexcept;
	uint16_t AddTexture(std::string name, std::string filePath) noexcept;
	DFSurface* GetTexture(std::string name) const noexcept;
	DFSurface* GetTexture(uint16_t index) const noexcept;
	uint16_t GetTextureIndex(std::string name) const noexcept;

	void DEBUG_ShowTextureIndex(uint16_t begin = 0, uint16_t end = 65535) noexcept;
};