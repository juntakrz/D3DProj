#include "DFMaterial.h"

// MATERIALS

uint16_t DFMaterial::MatAdd(DFMATERIAL_DESC* pDesc) noexcept
{
	uint16_t index = 0;

	for (const auto& it : m_Materials)
	{
		if (it->name == pDesc->name)
		{
			return index;
		}

		if (index < it->id)
		{
			break;
		}
		index++;
	}

	DFMaterial::Material newMat;
	newMat.id = index;
	newMat.name = pDesc->name;
	newMat.shaderPixel = pDesc->shaders.pixel;
	newMat.shaderVertex = pDesc->shaders.vertex;

	pDesc->textures.tex0 != "" ?
		newMat.idTex[0] = TextureAdd(pDesc->textures.tex0)
		: newMat.idTex[0] = TextureAdd("default//default.dds");

	pDesc->textures.tex1 != "" ?
		newMat.idTex[1] = TextureAdd(pDesc->textures.tex1)
		: newMat.idTex[1] = TextureAdd("default//default_n.dds");

	pDesc->textures.tex2 != "" ?
		newMat.idTex[2] = TextureAdd(pDesc->textures.tex2)
		: newMat.idTex[2] = TextureAdd("default//default_s.dds");

	pDesc->textures.tex3 != "" ?
		newMat.idTex[3] = TextureAdd(pDesc->textures.tex3)
		: newMat.idTex[3] = TextureAdd("default//default_s.dds");

	pDesc->textures.tex4 != "" ?
		newMat.idTex[4] = TextureAdd(pDesc->textures.tex4)
		: newMat.idTex[4] = TextureAdd("default//default_s.dds");

	pDesc->textures.tex5 != "" ?
		newMat.idTex[5] = TextureAdd(pDesc->textures.tex5)
		: newMat.idTex[5] = TextureAdd("default//default_s.dds");

	newMat.ambientColor = pDesc->material.ambientColor;
	newMat.data.x = pDesc->material.matIntensity;
	newMat.data.y = pDesc->material.spec_metal;
	newMat.data.z = pDesc->material.pow_roughness;

	// bump coefficient for shader needs to be reversed, so 2 times bump intensity will equal 0.5 in the shader
	float bumpCoef = 1.0f - ((pDesc->material.bumpIntensity - 1.0f) * 0.5);
	newMat.data.w = std::max(0.05f, bumpCoef);		// safeguard against zero or negative values

	newMat.F0 = pDesc->material.F0;
	newMat.manageTextures = pDesc->manageTextures;
	newMat.effects = pDesc->effects;

	m_Materials.emplace_back(std::make_unique<DFMaterial::Material>(std::move(newMat)));
	return index;
}

DFMaterial::Material& DFMaterial::Mat(std::string name) noexcept
{
	for (const auto& it : m_Materials)
	{
		if (name == it->name)
		{
			return *it;
		}
	}

#ifdef _DEBUG || _DFDEBUG
	name = "Material '" + name + "' not found.";
	OutputDebugStringA(name.c_str());
#endif
	return *m_Materials[0];
}

DFMaterial::Material& DFMaterial::Mat(uint16_t index) noexcept
{
	for (const auto& it : m_Materials)
	{
		if (it->id == index)
		{
			return *m_Materials[index];
		}
	}

#ifdef _DEBUG || _DFDEBUG
	OutputDebugStringA("Material index out of bounds.");
#endif
	return *m_Materials[0];
}

uint16_t DFMaterial::MatIndex(std::string name) const noexcept
{
	uint16_t index = 0;
	for (const auto& it : m_Materials)
	{
		if (name == it->name)
		{
			return index;
		}
		index++;
	}

#ifdef _DEBUG || _DFDEBUG
	name = "DFMaterial: Index for '" + name + "' not found.\n";
	OutputDebugStringA(name.c_str());
#endif
	return 0;
}

uint16_t DFMaterial::MatCount() const noexcept
{
	return m_Materials.size();
}

void DFMaterial::MatDelete(uint16_t index) noexcept {

	uint16_t it_n = 0;

	for (auto& it : m_Materials) {

		if (it->id == index && index > 0) {
			
			if (it->manageTextures) {

				for (uint32_t i = 0; i < sizeof(it->idTex) / sizeof(uint32_t); i++) {

					(it->idTex[i] > 0) ? TextureDelete(it->idTex[i]) : 0;
				}
			}
			m_Materials.erase(m_Materials.begin() + it_n);
			return;
		}
		it_n++;
	}
}

void DFMaterial::MatDelete(std::string name) noexcept
{
	uint16_t it_n = 0;

	for (auto& it : m_Materials) {

		if (it->name == name && it_n > 0) {

			if (it->manageTextures) {

				for (uint32_t i = 0; i < sizeof(it->idTex) / sizeof(uint32_t); i++) {

					(it->idTex[i] > 0) ? TextureDelete(it->idTex[i]) : 0;
				}
			}
			m_Materials.erase(m_Materials.begin() + it_n);
			return;
		}
		it_n++;
	}
}

// DEBUG

void DFMaterial::DEBUG_ShowMaterialIndex(uint16_t begin, uint16_t end) const noexcept 
{

	end > m_Materials.size() ? end = m_Materials.size() : 0;
	begin > end ? begin = end : 0;

	std::stringstream sstr;
	uint16_t index = 0;

	for (const auto& it : m_Materials) 
	{
		if (index > end)
		{
			break;
		}

		if (index >= begin)
		{
			sstr << "[" << index << ": " << it->id << "] " << it->name << ": ";
			for (uint32_t i = 0; i < sizeof(it->idTex) / sizeof(uint32_t); i++) {
				sstr << TextureGetName(it->idTex[i]) << ", ";
			}

			sstr << "\n";
		}

		index++;
	}

	MessageBoxA(nullptr, std::string(sstr.str()).c_str(), "DEBUG MaterialIndex", MB_OK | MB_ICONINFORMATION);
}

void DFMaterial::DEBUG_ShowTextureIndex(uint32_t begin, uint32_t end) const noexcept {

	end > m_DFTextures.size() ? end = m_DFTextures.size() : 0;
	begin > end ? begin = end : 0;

	std::stringstream sstr;
	uint32_t index = 0;

	for (const auto& it : m_DFTextures) {

		if (index > end) {
			break;
		}

		if (index >= begin) {
			sstr << "[" << index << ": " << it.id << "] " << it.name << ": " << it.filePath << "\n";
		}

		index++;
	}

	MessageBoxA(nullptr, std::string(sstr.str()).c_str(), "DEBUG TextureIndex", MB_OK | MB_ICONINFORMATION);
}