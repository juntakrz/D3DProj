#include "Bind_PixelShader.h"

namespace Bind
{
	PixelShader::PixelShader(D3DMgr& d3dMgr, const std::string& filePath) noexcept : m_FilePath(filePath.begin(), filePath.end())
	{
		D3D_DXGIDEBUG(d3dMgr);

		D3D_THROW(D3DReadFileToBlob(m_FilePath.c_str(), &m_pBlob));
		D3D_THROW(GetDevice(d3dMgr)->CreatePixelShader(m_pBlob->GetBufferPointer(), m_pBlob->GetBufferSize(), nullptr, &m_pPS));
	}

	void PixelShader::Bind(D3DMgr& d3dMgr) noexcept
	{
		GetContext(d3dMgr)->PSSetShader(m_pPS.Get(), nullptr, NULL);
	}
}