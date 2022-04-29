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
		: newMat.pTexBase = GetTexture(AddTexture("default//default.dds"));

	pDesc->textures.normal != "" ?
		newMat.pTexNormal = GetTexture(AddTexture(pDesc->textures.normal))
		: newMat.pTexNormal = GetTexture(AddTexture("default//default_n.dds"));

	pDesc->textures.tex2 != "" ?
		newMat.pTex2 = GetTexture(AddTexture(pDesc->textures.tex2))
		: newMat.pTex2 = GetTexture(AddTexture("default//default_s.dds"));

	pDesc->textures.tex3 != "" ?
		newMat.pTex3 = GetTexture(AddTexture(pDesc->textures.tex3))
		: newMat.pTex3 = GetTexture(AddTexture("default//default_s.dds"));

	pDesc->textures.tex4 != "" ?
		newMat.pTex4 = GetTexture(AddTexture(pDesc->textures.tex4))
		: newMat.pTex4 = GetTexture(AddTexture("default//default_s.dds"));

	pDesc->textures.tex5 != "" ?
		newMat.pTex5 = GetTexture(AddTexture(pDesc->textures.tex5))
		: newMat.pTex5 = GetTexture(AddTexture("default//default_s.dds"));

	newMat.ambientColor = pDesc->material.ambientColor;
	newMat.data.x = pDesc->material.matIntensity;
	newMat.data.y = pDesc->material.spec_metal;
	newMat.data.z = pDesc->material.pow_roughness;
	newMat.data.w = pDesc->material.extra;
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
	(stat(checkPath.c_str(), &buffer) == 0) ? filePath : filePath = "default//default.dds";
	/*if (stat(checkPath.c_str(), &buffer) != 0)
	{
		std::stringstream sstr;
		sstr << "File not found at " << checkPath;
		MessageBoxA(nullptr, std::string(sstr.str()).c_str(), "DFMaterial Error", MB_OK | MB_ICONWARNING);
	}*/

	for (const auto& it : m_DXTextures) {

		if (it.name == filePath || it.filePath == filePath) {
			return index;
		}
		index++;
	}

	DFMaterial::DXTexture dxt{};
	dxt.name = filePath;
	dxt.filePath = filePath;

	filePath = constPath + filePath;
	std::wstring wFilePath(filePath.begin(), filePath.end());

	HRESULT hr = DirectX::CreateDDSTextureFromFile(DFData::pD3DM->GetDevice(), wFilePath.c_str(), nullptr, &dxt.pSRV);

	if (hr != S_OK)
	{
		std::stringstream sstr;
		sstr << "Error #" << hr << "" << "reading DDS texture at " << filePath;
		MessageBoxA(nullptr, std::string(sstr.str()).c_str(), "DFMaterial Error", MB_OK | MB_ICONWARNING);
	}

	m_DXTextures.emplace_back(std::move(dxt));
	return index;
}

ID3D11ShaderResourceView* DFMaterial::GetTexture(uint16_t index) noexcept
{
	if (index < m_DXTextures.size()) {
		return m_DXTextures[index].pSRV;
	}
	else {
		return m_DXTextures[0].pSRV;
	}
}

uint16_t DFMaterial::AddTexturePNG(std::string filePath) noexcept {

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

uint16_t DFMaterial::AddTexturePNG(std::string name, std::string filePath) noexcept {

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

DFSurface* DFMaterial::GetTexturePNG(std::string name) const noexcept {

	for (const auto& it : m_Textures) {

		if (it->name == name || it->filePath == name) {
			return &it->texture;
		}
	}

	return GetTexturePNG("default//default.png");
}

DFSurface* DFMaterial::GetTexturePNG(uint16_t index) const noexcept {

	if (index < m_Textures.size()) {
		return &m_Textures[index]->texture;
	}
}

uint16_t DFMaterial::GetTextureIndexPNG(std::string name) const noexcept {

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

	end > m_DXTextures.size() ? end = m_DXTextures.size() : 0;
	begin > end ? begin = end : 0;

	std::stringstream sstr;
	uint16_t index = 0;

	for (const auto& it : m_DXTextures) {

		if (index > end) {
			break;
		}

		if (index >= begin) {
			sstr << "[" << index << "] " << it.name << ": " << it.filePath << "\n";
		}

		index++;
	}

	MessageBoxA(nullptr, std::string(sstr.str()).c_str(), "DEBUG TextureIndex", MB_OK | MB_ICONINFORMATION);
}
