#include "Bind_PixelShader.h"

namespace Bind
{
	PixelShader::PixelShader(const std::string& filePath) noexcept : m_FilePath(filePath.begin(), filePath.end())
	{
		D3D_DXGIDEBUG(*DF::DFM);

		D3D_THROW(D3DReadFileToBlob(m_FilePath.c_str(), &m_pBlob));
		D3D_THROW(GetDevice()->CreatePixelShader(m_pBlob->GetBufferPointer(), m_pBlob->GetBufferSize(), nullptr, &m_pPS));
	}

	void PixelShader::Bind() noexcept
	{
		GetContext()->PSSetShader(m_pPS.Get(), nullptr, NULL);
	}
	void PixelShader::Unbind() noexcept
	{
		GetContext()->PSSetShader(nullptr, nullptr, NULL);
	}
}