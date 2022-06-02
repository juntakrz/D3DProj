#include "DFMaterial.h"

// TEXTURES

std::string DFMaterial::TextureAdd(std::string filePath) noexcept {

	return DFMaterial::TextureAdd(filePath, filePath);
}

std::string DFMaterial::TextureAdd(std::string name, std::string filePath) noexcept {

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

	if (m_DFTextures.find(name) == m_DFTextures.end())
	{
		DFMaterial::DFTexture dft{};
		dft.name = name;
		dft.filePath = filePath;

		filePath = constPath + filePath;
		std::wstring wFilePath(filePath.begin(), filePath.end());

		dft.pSRV = std::make_shared<ID3D11ShaderResourceView*>(nullptr);

		HRESULT hr = DirectX::CreateDDSTextureFromFileEx(
			DF::D3DM->Device(), nullptr, wFilePath.c_str(), 0uLL,
			D3D11_USAGE_DEFAULT, D3D11_BIND_SHADER_RESOURCE,
			0u, 0u, false, nullptr, dft.pSRV.get()
		);

		m_DFTextures[name] = std::move(dft);

#ifdef _DEBUG || _DFDEBUG
		if (hr != S_OK)
		{
			std::stringstream sstr;
			sstr << "Error #" << hr << "" << "reading DDS texture at " << filePath << "\n";
			OutputDebugStringA(std::string(sstr.str()).c_str());
		}
#endif
	}
	return name;
}

std::string DFMaterial::TextureAddRT(std::string name, UINT width, UINT height) noexcept
{
	DFTexture newRTTex{};
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

	DF::D3DM->Device()->CreateTexture2D(&texDesc, nullptr, &newRTTex.pTex);

	D3D11_RENDER_TARGET_VIEW_DESC RTVDesc{};
	RTVDesc.Format = texDesc.Format;
	RTVDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
	RTVDesc.Texture2D.MipSlice = 0;

	DF::D3DM->Device()->CreateRenderTargetView(newRTTex.pTex, &RTVDesc, &newRTTex.pRTV);

	D3D11_SHADER_RESOURCE_VIEW_DESC SRVDesc{};
	SRVDesc.Format = texDesc.Format;
	SRVDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	SRVDesc.Texture2D.MipLevels = 1;
	SRVDesc.Texture2D.MostDetailedMip = 0;

	DF::D3DM->Device()->CreateShaderResourceView(newRTTex.pTex, &SRVDesc, &*newRTTex.pSRV);

	bool result = m_DFTextures.try_emplace(name, std::move(newRTTex)).second;

#ifdef _DEBUG || _DFDEBUG
	if (!result)
	{
		std::stringstream sstr;
		sstr << "DFMaterial Error: failed to add RTTexture '" << name.c_str() << "'.\n";
		OutputDebugStringA(std::string(sstr.str()).c_str());
	}
#endif

	return name;
}

std::shared_ptr<ID3D11ShaderResourceView*> DFMaterial::TextureGet(const std::string& name) const noexcept
{
	// to make this method faster - has no check if texture exists, use TextureExists() for safer method
	return m_DFTextures.at(name).pSRV;
}

bool DFMaterial::TextureExists(const std::string& name) const noexcept
{
	return (m_DFTextures.find(name) != m_DFTextures.end());
}

bool DFMaterial::TextureIsRT(const std::string& name) const noexcept
{
	if (m_DFTextures.find(name) != m_DFTextures.end())
	{
		if (m_DFTextures.at(name).pRTV != nullptr)
		{
			return true;
		}
	}

	return false;
}

bool DFMaterial::TextureDelete(std::string name) noexcept
{
	// should never delete default textures, also return false if texture does not exist
	if (name == "default//default.dds"
		|| name == "default//default_n.dds"
		|| name == "default//default_s.dds"
		|| m_DFTextures.find(name) == m_DFTextures.end()
		)
	{
		return false;
	}
		
	if (m_DFTextures.at(name).pSRV.use_count() < 2)
	{
		auto SRV = *m_DFTextures.at(name).pSRV.get();
		SRV->Release();
		m_DFTextures.at(name).pSRV = nullptr;

		if (m_DFTextures.at(name).pRTV) {
			m_DFTextures.at(name).pRTV->Release();
			m_DFTextures.at(name).pRTV = nullptr;
		}

		m_DFTextures.erase(name);

		return true;
	}
}

bool DFMaterial::BindTextureToPS(const std::string& texture, const UINT& slot, const UINT& num) noexcept
{
	if (m_DFTextures.find(texture) != m_DFTextures.end())
	{
		DF::Context()->PSSetShaderResources(slot, num, m_DFTextures.at(texture).pSRV.get());
		return true;
	}

#ifdef _DEBUG || _DFDEBUG
	std::string msg = "DFMaterial: couldn't bind '" + texture + "' as a pixel shader resource.\n";
	OutputDebugStringA(msg.c_str());
#endif

	return false;
}

bool DFMaterial::BindTextureToGS(const std::string& texture, const UINT& slot, const UINT& num) noexcept
{
	if (m_DFTextures.find(texture) != m_DFTextures.end())
	{
		DF::Context()->PSSetShaderResources(slot, num, m_DFTextures.at(texture).pSRV.get());
		return true;
	}

#ifdef _DEBUG || _DFDEBUG
	std::string msg = "DFMaterial: couldn't bind '" + texture + "' as a geometry shader resource.\n";
	OutputDebugStringA(msg.c_str());
#endif

	return false;
}