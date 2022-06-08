#include "DFMaterial.h"

void* DFMaterial::ShaderAdd(const std::string& filePath) noexcept
{
	// correct naming check below, maybe needs a better implementation?
	std::string path = filePath;

	// if shader name contains path/subfolder to shader - recursively strip it
	if (filePath[0] != 'V' && filePath[0] != 'G' && filePath[0] != 'P')
	{
		std::istringstream iss(filePath);
		while (std::getline(iss, path, '/')) {};
	}

	// determine which type of shader it is by the first letter (requires correct/strict shader naming)
	switch (path[0])
	{
	case 'V':		// vertex shader
	{
		// emplace if not present, return ptr to shader code if present
		if (!m_Shaders.try_emplace(filePath).second)
		{
			return m_Shaders.at(filePath).pVS.Get();
		}

		// get a pointer to newly created shader
		DFShader* pShader = &m_Shaders.at(filePath);

		// convert to a full path
		std::wstring wpath = L"shaders/" + std::wstring(filePath.begin(), filePath.end()) + L".shd";

		// write shader to blob
		D3DReadFileToBlob(wpath.c_str(), &pShader->pSData);

		// create vertex shader
		DF::Device()->CreateVertexShader(pShader->pSData->GetBufferPointer(), pShader->pSData->GetBufferSize(), nullptr, &pShader->pVS);

		return pShader->pVS.Get();
	}
	case 'G':		// geometry shader
	{
		// emplace if not present, return ptr to shader code if present
		if (!m_Shaders.try_emplace(filePath).second)
		{
			return m_Shaders.at(filePath).pGS.Get();
		}

		// get a pointer to newly created shader
		DFShader* pShader = &m_Shaders.at(filePath);

		// convert to a full path
		std::wstring wpath = L"shaders/" + std::wstring(filePath.begin(), filePath.end()) + L".shd";

		// write shader to blob
		D3DReadFileToBlob(wpath.c_str(), &pShader->pSData);

		// create geometry shader
		DF::Device()->CreateGeometryShader(pShader->pSData->GetBufferPointer(), pShader->pSData->GetBufferSize(), nullptr, &pShader->pGS);

		return pShader->pGS.Get();
	}
	case 'P':		// pixel shader
	{
		// emplace if not present, return ptr to shader code if present
		if (!m_Shaders.try_emplace(filePath).second)
		{
			return m_Shaders.at(filePath).pPS.Get();
		}

		// get a pointer to newly created shader
		DFShader* pShader = &m_Shaders.at(filePath);

		// convert to a full path
		std::wstring wpath = L"shaders/" + std::wstring(filePath.begin(), filePath.end()) + L".shd";

		// write shader to blob
		D3DReadFileToBlob(wpath.c_str(), &pShader->pSData);

		// create vertex shader
		DF::Device()->CreatePixelShader(pShader->pSData->GetBufferPointer(), pShader->pSData->GetBufferSize(), nullptr, &pShader->pPS);

		return pShader->pPS.Get();
	}
	}
}

void* DFMaterial::ShaderGet(const std::string& id) const noexcept
{
	// determine which type of shader it is by the first letter (requires correct/strict shader naming)
	// id is the same thing as filePath from the previous method
	
	std::string filteredId(id);

	// if shader name contains path/subfolder to shader - recursively strip it
	if (id[0] != 'V' && id[0] != 'G' && id[0] != 'P')
	{
		std::istringstream iss(id);
		while (std::getline(iss, filteredId, '/')) {};
	}

	switch (filteredId[0])
	{
	case 'V':		// vertex shader
	{
		if (m_Shaders.find(id) != m_Shaders.end())
		{
			return m_Shaders.at(id).pVS.Get();
		}

#ifdef _DEBUG || _DFDEBUG
		std::string msg = "ERROR: DFMaterial: vertex shader '" + id + "' not found. Returning nullptr.\n";	// or should simply use ShaderAdd() instead of returning nullptr?
		OutputDebugStringA(msg.c_str());
#endif
		return nullptr;
	}
	case 'G':		// geometry shader
	{
		if (m_Shaders.find(id) != m_Shaders.end())
		{
			return m_Shaders.at(id).pGS.Get();
		}

#ifdef _DEBUG || _DFDEBUG
		std::string msg = "ERROR: DFMaterial: geometry shader '" + id + "' not found. Returning nullptr.\n";
		OutputDebugStringA(msg.c_str());
#endif
		return nullptr;
	}
	case 'P':		// pixel shader
	{
		if (m_Shaders.find(id) != m_Shaders.end())
		{
			return m_Shaders.at(id).pPS.Get();
		}

#ifdef _DEBUG || _DFDEBUG
		std::string msg = "ERROR: DFMaterial: pixel shader '" + id + "' not found. Returning nullptr.\n";
		OutputDebugStringA(msg.c_str());
#endif
		return nullptr;
	}
	}
}

ID3DBlob* DFMaterial::ShaderByteCode(const std::string& id) const noexcept
{
	/*
	if (m_Shaders.find(id) != m_Shaders.end())
	{
		return m_Shaders.at(id).pSData.Get();
	}

#ifdef _DEBUG || _DFDEBUG
	std::string msg = "ERROR: DFMaterial: unable to find byte code for '" + id + "'. Returning nullptr.\n";	// or should simply use ShaderAdd() instead of returning nullptr?
	OutputDebugStringA(msg.c_str());
#endif
	return nullptr;
	*/

	// for speed this method will not check for errors as it assumes the caller has the correct shader name due to prior shader creation
	return m_Shaders.at(id).pSData.Get();
}

bool DFMaterial::ShaderDelete(const std::string& id) noexcept
{
	if (m_Shaders.find(id) != m_Shaders.end())
	{
		auto& pShader = m_Shaders.at(id);

		(pShader.pVS) ? pShader.pVS->Release() : 0;
		(pShader.pGS) ? pShader.pGS->Release() : 0;
		(pShader.pPS) ? pShader.pPS->Release() : 0;
		(pShader.pSData) ? pShader.pSData->Release() : 0;

		m_Shaders.erase(id);
		return true;
	}

#ifdef _DEBUG || _DFDEBUG
	std::string msg = "ERROR: DFMaterial: could not delete shader '" + id + "'. Not found.\n";
	OutputDebugStringA(msg.c_str());
#endif

	return false;
}
