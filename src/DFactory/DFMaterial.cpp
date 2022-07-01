#include "DFMaterial.h"

// MATERIALS

DFMaterial::Material& DFMaterial::Mat(const char* name) noexcept
{
	if (m_Materials.find(name) == m_Materials.end()) {
#ifdef _DEBUG || _DFDEBUG
		std::string msg = "Material '" + std::string(name) + "' not found.";
		OutputDebugStringA(msg.c_str());
#endif
		return *m_Materials.at(DF::Default::material);
	}

	return *m_Materials.at(name);
}

DFMaterial::Material& DFMaterial::MatAdd(DFMATERIAL_DESC* pDesc) noexcept
{
	// if material already exists - return the reference to an existing one
	if (!m_Materials.try_emplace(pDesc->name).second) {
		return *m_Materials.at(pDesc->name);
	}

	DFMaterial::Material newMat;

	newMat.name = pDesc->name;
	newMat.shaderPixel = pDesc->shaders.pixel;
	newMat.shaderGeometry = pDesc->shaders.geometry;
	newMat.shaderVertex = pDesc->shaders.vertex;

	pDesc->textures.tex0 != "" ?
		newMat.idTex[0] = TextureAdd(pDesc->textures.tex0)
		: newMat.idTex[0] = TextureAdd("default//default.dds");

	pDesc->textures.tex1 != "" ?
		newMat.idTex[1] = TextureAdd(pDesc->textures.tex1)
		: newMat.idTex[1] = TextureAdd("default//default_n.dds");

	pDesc->textures.tex2 != "" ?
		newMat.idTex[2] = TextureAdd(pDesc->textures.tex2)
		: newMat.idTex[2] = TextureAdd("default//default_metallic.dds");

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
	newMat.data.x = pDesc->material.intensity;
	newMat.data.y = pDesc->material.metalness;
	newMat.data.z = pDesc->material.roughness;

	// bump coefficient for shader needs to be reversed, so 2 times bump intensity will equal 0.5 in the shader
	float bumpCoef = 1.0f - ((pDesc->material.bumpiness - 1.0f) * 0.5);
	newMat.data.w = std::max(0.05f, bumpCoef);		// safeguard against zero or negative values

	newMat.F0 = pDesc->material.F0;
	newMat.manageTextures = pDesc->manageTextures;
	newMat.passes = pDesc->passes;

	m_Materials.at(pDesc->name) = std::make_unique<DFMaterial::Material>(std::move(newMat));
	return *m_Materials.at(pDesc->name);
}

bool DFMaterial::MatDelete(const char* name) noexcept
{
	if (m_Materials.find(name) == m_Materials.end()) {
#ifdef _DEBUG || _DFDEBUG
		std::string msg = "Material '" + std::string(name) + "' does not exist, could not delete.";
		OutputDebugStringA(msg.c_str());
#endif
		return false;
	}
	
	if (m_Materials.at(name)->manageTextures) {

		for (uint32_t i = 0; i < DF::maxTextures; i++) {

			(m_Materials.at(name)->idTex[i] != "") ? TextureDelete(m_Materials.at(name)->idTex[i]) : 0;
		}
	}
	m_Materials.erase(name);
	return true;
}

bool DFMaterial::MatExists(const char* name) noexcept
{
	return (m_Materials.find(name) == m_Materials.end()) ? false : true;
}

uint16_t DFMaterial::MatCount() const noexcept
{
	return m_Materials.size();
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
			sstr << "[" << it.second->name << "]: ";
			for (uint32_t i = 0; i < 6; i++) {
				sstr << it.second->idTex[i].c_str() << ", ";
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
			sstr << "[" << index << "]: " << it.second.name << ": " << it.second.filePath << "\n";
		}

		index++;
	}

	MessageBoxA(nullptr, std::string(sstr.str()).c_str(), "DEBUG TextureIndex", MB_OK | MB_ICONINFORMATION);
}