#include "Bind_VertexShader.h"

namespace Bind
{
	VertexShader::VertexShader(const std::string& filePath) noexcept : m_FilePath(filePath.begin(), filePath.end())
	{
		D3D_DXGIDEBUG(*DF::D3DM);

		D3D_THROW(D3DReadFileToBlob(m_FilePath.c_str(), &m_pBlob));
		D3D_THROW(GetDevice()->CreateVertexShader(m_pBlob->GetBufferPointer(), m_pBlob->GetBufferSize(), nullptr, &m_pVS));
	}

	void VertexShader::Bind() noexcept
	{
		GetContext()->VSSetShader(m_pVS.Get(), nullptr, NULL);
	}

	ID3DBlob* VertexShader::GetByteCode() const noexcept
	{
		return m_pBlob.Get();
	}
}