#include "DFMaterial.h"

// TEXTURES

uint32_t DFMaterial::TextureAdd(std::string filePath) noexcept {

	return DFMaterial::TextureAdd(filePath, filePath);
}

uint32_t DFMaterial::TextureAdd(std::string name, std::string filePath) noexcept {

	uint16_t index = 0;

	std::string checkPath = constPath + filePath;
	struct stat buffer;
	(stat(checkPath.c_str(), &buffer) == 0) ? filePath : filePath = "default//default.dds";

#ifdef _DEBUG || _DFDEBUG
	if (stat(checkPath.c_str(), &buffer) != 0)
	{
		std::stringstream sstr;
		sstr << "DFMaterial Error: File not found at " << checkPath << "\n";
		OutputDebugStringA(std::string(sstr.str()).c_str());
	}
#endif

	for (const auto& it : m_DFTextures) {

		if (it.name == name || it.filePath == filePath) {
			return it.id;
		}

		if (index < it.id) {	//if there is a free id - use it
			break;
		}
		index++;
	}

	DFMaterial::DFTexture dft{};
	dft.id = index;
	dft.name = name;
	dft.filePath = filePath;

	filePath = constPath + filePath;
	std::wstring wFilePath(filePath.begin(), filePath.end());

	dft.pSRV = std::make_shared<ID3D11ShaderResourceView*>(nullptr);

	//HRESULT hr = DirectX::CreateDDSTextureFromFile(DF::D3DM->Device(), wFilePath.c_str(),nullptr, dft.pSRV.get());
	HRESULT hr = DirectX::CreateDDSTextureFromFileEx(
		DF::D3DM->Device(), nullptr, wFilePath.c_str(), 0uLL,
		D3D11_USAGE_DEFAULT, D3D11_BIND_SHADER_RESOURCE,
		0u, 0u, false, nullptr, dft.pSRV.get()
		);

	// diagnosis code, used to peek inside imported DDS resource
	/*
	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc{};
	ID3D11ShaderResourceView* srv = *dft.pSRV.get();
	srv->GetDesc(&srvDesc);

	ID3D11Resource* res;
	srv->GetResource(&res);

	ID3D11Texture2D* tex = reinterpret_cast<ID3D11Texture2D*>(res);
	D3D11_TEXTURE2D_DESC texDesc{};
	tex->GetDesc(&texDesc);
	*/

#ifdef _DEBUG || _DFDEBUG
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

uint32_t DFMaterial::TextureAddRT(std::string name, UINT width, UINT height) noexcept
{
	uint32_t index = 0;
	for (const auto& it : m_DFTextures)
	{
		if (index < it.id)
		{
			break;
		}
		index++;
	}

	COMPTR<ID3D11Texture2D> m_pTex2D;

	DFTexture newRTTex{};
	newRTTex.id = index;
	newRTTex.name = name;
	newRTTex.filePath = "nullpath";
	newRTTex.pSRV = std::make_shared<ID3D11ShaderResourceView*>();
	newRTTex.pRTV = nullptr;

	D3D11_TEXTURE2D_DESC texDesc{};
	texDesc.Width = width;
	texDesc.Height = height;
	texDesc.MipLevels = 1;
	texDesc.ArraySize = 1;
	texDesc.SampleDesc.Count = 1;
	texDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	texDesc.Usage = D3D11_USAGE_DEFAULT;
	texDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;
	texDesc.CPUAccessFlags = 0;
	texDesc.MiscFlags = 0;

	DF::D3DM->Device()->CreateTexture2D(&texDesc, nullptr, &m_pTex2D);

	D3D11_RENDER_TARGET_VIEW_DESC RTVDesc{};
	RTVDesc.Format = texDesc.Format;
	RTVDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
	RTVDesc.Texture2D.MipSlice = 0;

	DF::D3DM->Device()->CreateRenderTargetView(m_pTex2D.Get(), &RTVDesc, &newRTTex.pRTV);

	D3D11_SHADER_RESOURCE_VIEW_DESC SRVDesc{};
	SRVDesc.Format = texDesc.Format;
	SRVDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	SRVDesc.Texture2D.MipLevels = 1;
	SRVDesc.Texture2D.MostDetailedMip = 0;

	DF::D3DM->Device()->CreateShaderResourceView(m_pTex2D.Get(), &SRVDesc, &*newRTTex.pSRV);

	m_DFTextures.emplace_back(std::move(newRTTex));

	return index;
}

std::shared_ptr<ID3D11ShaderResourceView*> DFMaterial::TextureGet(uint16_t index) const noexcept {
	if (index < m_DFTextures.size()) {
		return m_DFTextures[index].pSRV;
	}
	else {
		return m_DFTextures[0].pSRV;
	}
}

std::shared_ptr<ID3D11ShaderResourceView*> DFMaterial::TextureGet(std::string name) const noexcept
{

	for (const auto& it : m_DFTextures) {

		if (it.name == name || it.filePath == name) {
			return it.pSRV;
		}
	}
	return m_DFTextures[0].pSRV;
}

std::string DFMaterial::TextureGetName(uint32_t index) const noexcept
{
	for (const auto& it : m_DFTextures) {
		if (index == it.id) {
			return it.name;
		}
	}

	return "Index OOB";
}

uint32_t DFMaterial::TextureGetIndex(std::string name) const noexcept
{

	for (const auto& it : m_DFTextures) {

		if (it.name == name || it.filePath == name) {
			return it.id;
		}

	}

	return 0;	//non-existent texture always defaults to checkerboard
}

bool DFMaterial::TextureExists(uint32_t index) const noexcept
{
	for (const auto& it : m_DFTextures) 
	{
		if (index == it.id)
		{
			return true;
		}
	}

	return false;
}

bool DFMaterial::TextureIsRT(uint32_t index) const noexcept
{
	for (const auto& it : m_DFTextures)
	{
		if (index == it.id)
		{
			bool isRTT = false;
			(it.pRTV != nullptr) ? isRTT = true : isRTT = false;
			return isRTT;
		}
	}

	return false;
}

bool DFMaterial::TextureDelete(uint32_t index) noexcept
{

	uint32_t it_n = 0;

	for (auto& it : m_DFTextures) 
	{

		if (index == it.id) 
		{

			if (it.pSRV.use_count() < 2) 
			{
				auto SRV = *it.pSRV.get();
				SRV->Release();
				it.pSRV = nullptr;

				if (it.pRTV) {
					it.pRTV->Release();
					it.pRTV = nullptr;
				}

				m_DFTextures.erase(m_DFTextures.begin() + it_n);

				return true;
			}
		}
		it_n++;
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