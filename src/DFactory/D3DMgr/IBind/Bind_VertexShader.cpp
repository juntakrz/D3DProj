#include "Bind_VertexShader.h"

namespace Bind
{
	VertexShader::VertexShader(D3DMgr& d3dMgr, const std::string& filePath) noexcept : m_FilePath(filePath.begin(), filePath.end())
	{
		D3D_DXGIDEBUG(d3dMgr);

		D3D_THROW(D3DReadFileToBlob(m_FilePath.c_str(), &m_pBlob));
		D3D_THROW(GetDevice(d3dMgr)->CreateVertexShader(m_pBlob->GetBufferPointer(), m_pBlob->GetBufferSize(), nullptr, &m_pVS));
	}

	void VertexShader::Bind(D3DMgr& d3dMgr) noexcept
	{
		GetContext(d3dMgr)->VSSetShader(m_pVS.Get(), nullptr, NULL);
	}

	ID3DBlob* VertexShader::GetByteCode() const noexcept
	{
		return m_pBlob.Get();
	}
}