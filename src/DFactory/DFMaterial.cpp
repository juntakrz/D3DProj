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
	newMat.data.w = pDesc->material.extra;
	newMat.F0 = pDesc->material.F0;
	newMat.manageTextures = pDesc->manageTextures;

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

#ifdef _DEBUG || SETDEBUG
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

	if (index < m_Materials.size() && index > 0) {

		if (m_Materials[index]->manageTextures) {
			
			for (uint16_t i = 0; i < sizeof(m_Materials[index]->idTex) / sizeof(uint16_t); i++) {

				(m_Materials[index]->idTex[i] > 0) ? TextureDelete(m_Materials[index]->idTex[i]) : 0;

			}

		}

		m_Materials.erase(m_Materials.begin() + index);
	}
}

// TEXTURES

uint16_t DFMaterial::TextureAdd(std::string filePath) noexcept {
		
	return DFMaterial::TextureAdd(filePath, filePath);
}

uint16_t DFMaterial::TextureAdd(std::string name, std::string filePath) noexcept {

	uint16_t index = 0;

	std::string checkPath = constPath + filePath;
	struct stat buffer;
	(stat(checkPath.c_str(), &buffer) == 0) ? filePath : filePath = "default//default.dds";

#ifdef _DEBUG || SETDEBUG
	if (stat(checkPath.c_str(), &buffer) != 0)
	{
		std::stringstream sstr;
		sstr << "DFMaterial Error: File not found at " << checkPath << "\n";
		OutputDebugStringA(std::string(sstr.str()).c_str());
	}
#endif

	for (const auto& it : m_DFTextures) {

		if (it.name == name || it.filePath == filePath) {
			return index;
		}
		index++;
	}

	DFMaterial::DFTexture dft{};
	dft.name = name;
	dft.filePath = filePath;

	filePath = constPath + filePath;
	std::wstring wFilePath(filePath.begin(), filePath.end());

	ID3D11ShaderResourceView* pSRV = nullptr;

	HRESULT hr = DirectX::CreateDDSTextureFromFile(DFData::pD3DM->GetDevice(), wFilePath.c_str(), nullptr, &pSRV);
	/*HRESULT hr = DirectX::CreateDDSTextureFromFileEx(
		DFData::pD3DM->GetDevice(), nullptr, wFilePath.c_str(), 0uLL,
		D3D11_USAGE_DEFAULT, D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET,
		0u, D3D11_RESOURCE_MISC_GENERATE_MIPS, false, nullptr, &dxt.pSRV
		);*/

	dft.pSRV = std::make_shared<ID3D11ShaderResourceView*>(pSRV);

#ifdef _DEBUG || SETDEBUG
	if (hr != S_OK)
	{
		std::stringstream sstr;
		sstr << "Error #" << hr << "" << "reading DDS texture at " << filePath << "\n";
		OutputDebugStringA(std::string(sstr.str()).c_str());
	}
#endif

	m_DFTextures.emplace_back(std::move(dft));
	return index;
}

ID3D11ShaderResourceView* DFMaterial::TextureGet(uint16_t index) const noexcept {
	if (index < m_DFTextures.size()) {
		return *m_DFTextures[index].pSRV.get();
	}
	else {
		return *m_DFTextures[0].pSRV.get();
	}
}

ID3D11ShaderResourceView* DFMaterial::TextureGet(std::string name) const noexcept {

	for (const auto& it : m_DFTextures) {

		if (it.name == name || it.filePath == name) {
			return *it.pSRV.get();
		}
	}
	return *m_DFTextures[0].pSRV.get();
}

std::string DFMaterial::TextureGetName(uint16_t index) const noexcept {
	if(index < m_DFTextures.size()) {
		return m_DFTextures[index].name;
	}

	return "Index OOB";
}

uint16_t DFMaterial::TextureGetIndex(std::string name) const noexcept {

	uint16_t index = 0;
	for (const auto& it : m_DFTextures) {

		if (it.name == name || it.filePath == name) {
			return index;
		}
		index++;
	}

	return 0;	//non-existent texture always defaults to checkerboard
}

bool DFMaterial::TextureDelete(uint16_t index) noexcept {
	if (index < m_DFTextures.size() && index > 2) {

		/* instead of deleting texture and causing a mess with IDs
		free the D3D resource and mark it as ready for overwriting*/

		if (m_DFTextures[index].pSRV.use_count() < 2) {
			m_DFTextures[index].name = "!VOID!";
			m_DFTextures[index].filePath = "nullpath";
			m_DFTextures[index].pSRV = nullptr;
			return true;
		}
	}
	return false;
}

bool DFMaterial::TextureDelete(std::string name) noexcept {

	// skip default textures
	uint16_t index = 3;

	for (const auto& it : m_DFTextures) {
		if (it.name == name || it.filePath == name) {
			return TextureDelete(index);
		}
		index++;
	}

	return false;
}

void DFMaterial::DEBUG_ShowMaterialIndex(uint16_t begin, uint16_t end) const noexcept {

	end > m_Materials.size() ? end = m_Materials.size() : 0;
	begin > end ? begin = end : 0;

	std::stringstream sstr;
	uint16_t index = 0;

	for (const auto& it : m_Materials) {

		if (index > end) {
			break;
		}

		if (index >= begin) {

			sstr << "[" << index << "] " << it->name << ": ";
			for (uint16_t i = 0; i < sizeof(it->idTex) / sizeof(uint16_t); i++) {
				sstr << TextureGetName(it->idTex[i]) << ", ";
			}

			sstr << "\n";
		}

		index++;
	}

	MessageBoxA(nullptr, std::string(sstr.str()).c_str(), "DEBUG MaterialIndex", MB_OK | MB_ICONINFORMATION);
}

void DFMaterial::DEBUG_ShowTextureIndex(uint16_t begin, uint16_t end) const noexcept {

	end > m_DFTextures.size() ? end = m_DFTextures.size() : 0;
	begin > end ? begin = end : 0;

	std::stringstream sstr;
	uint16_t index = 0;

	for (const auto& it : m_DFTextures) {

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
