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
		index++;
	}

	DFMaterial::Material newMat;
	newMat.name = pDesc->name;
	newMat.shaderPixel = pDesc->shaders.pixel;
	newMat.shaderVertex = pDesc->shaders.vertex;

	pDesc->textures.base != "" ?
		newMat.pTexBase = GetTexture(AddTexture(pDesc->textures.base))
		: newMat.pTexBase = GetTexture(AddTexture("default//default.png"));

	pDesc->textures.normal != "" ?
		newMat.pTexNormal = GetTexture(AddTexture(pDesc->textures.normal))
		: newMat.pTexNormal = GetTexture(AddTexture("default//default_n.png"));

	pDesc->textures.tex2 != "" ?
		newMat.pTex2 = GetTexture(AddTexture(pDesc->textures.tex2))
		: newMat.pTex2 = GetTexture(AddTexture("default//default_s.png"));

	pDesc->textures.tex3 != "" ?
		newMat.pTex3 = GetTexture(AddTexture(pDesc->textures.tex3))
		: newMat.pTex3 = GetTexture(AddTexture("default//default_s.png"));

	pDesc->textures.tex4 != "" ?
		newMat.pTex4 = GetTexture(AddTexture(pDesc->textures.tex4))
		: newMat.pTex4 = GetTexture(AddTexture("default//default_s.png"));

	pDesc->textures.tex5 != "" ?
		newMat.pTex5 = GetTexture(AddTexture(pDesc->textures.tex5))
		: newMat.pTex5 = GetTexture(AddTexture("default//default_s.png"));

	newMat.ambientColor = pDesc->material.ambientColor;
	newMat.data.x = pDesc->material.matIntensity;
	newMat.data.y = pDesc->material.reflectivity;
	newMat.data.z = pDesc->material.pow_roughness;
	newMat.F0 = pDesc->material.F0;

	m_Materials.push_back(std::make_unique<DFMaterial::Material>(std::move(newMat)));
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

	name = "Material '" + name + "' not found.";
	MessageBoxA(nullptr, name.c_str(), "DFMaterial Error", MB_OK | MB_ICONWARNING);
	return *m_Materials[0];
}

DFMaterial::Material& DFMaterial::Mat(uint16_t index) noexcept
{
	if (index < m_Materials.size())
	{
		return *m_Materials[index];
	}

	MessageBoxA(nullptr, "Index out of bounds.", "DFMaterial Error", MB_OK | MB_ICONWARNING);
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

	name = "Index for '" + name + "' not found.";
	MessageBoxA(nullptr, name.c_str(), "DFMaterial Error", MB_OK | MB_ICONWARNING);
	return 0;
}

// TEXTURES

uint16_t DFMaterial::AddTexture(std::string filePath) noexcept {

	uint16_t index = 0;

	std::string checkPath = constPath + filePath;
	struct stat buffer;
	(stat(checkPath.c_str(), &buffer) == 0) ? filePath : filePath = "default//default.png";

	for (const auto& it : m_Textures) {

		if (it->name == filePath || it->filePath == filePath) {
			return index;
		}
		index++;
	}

	m_Textures.push_back(std::make_unique<Texture>(filePath, filePath, DFSurface::LoadFromFile(constPath + filePath)));
	return index;
}

uint16_t DFMaterial::AddTexture(std::string name, std::string filePath) noexcept {

	uint16_t index = 0;
	
	std::string checkPath = constPath + filePath;
	struct stat buffer;
	(stat(checkPath.c_str(), &buffer) == 0) ? filePath : filePath = "default//default.png";

	for (const auto& it : m_Textures) {

		if (it->name == name || it->filePath == filePath) {
			return index;
		}
		index++;
	}

	m_Textures.push_back(std::make_unique<Texture>(name, filePath, DFSurface::LoadFromFile(constPath + filePath)));
	return index;
}

DFSurface* DFMaterial::GetTexture(std::string name) const noexcept {

	for (const auto& it : m_Textures) {

		if (it->name == name || it->filePath == name) {
			return &it->texture;
		}
	}

	return GetTexture("default//default.png");
}

DFSurface* DFMaterial::GetTexture(uint16_t index) const noexcept {

	if (index < m_Textures.size()) {
		return &m_Textures[index]->texture;
	}
}

uint16_t DFMaterial::GetTextureIndex(std::string name) const noexcept {

	uint16_t index = 0;
	for (const auto& it : m_Textures) {

		if (it->name == name || it->filePath == name) {
			return index;
		}
		index++;
	}

	return 0;	//non-existent texture always defaults to checkerboard
}

void DFMaterial::DEBUG_ShowTextureIndex(uint16_t begin, uint16_t end) noexcept {

	end > m_Textures.size() ? end = m_Textures.size() : 0;
	begin > end ? begin = end : 0;

	std::stringstream sstr;
	uint16_t index = 0;

	for (const auto& it : m_Textures) {

		if (index > end) {
			break;
		}

		if (index >= begin) {
			sstr << "[" << index << "] " << it->name << ": " << it->filePath << "\n";
		}

		index++;
	}

	MessageBoxA(nullptr, std::string(sstr.str()).c_str(), "DEBUG TextureIndex", MB_OK | MB_ICONINFORMATION);
}
