#include "Bind_GeometryShader.h"

namespace Bind
{
	GeometryShader::GeometryShader(const std::string& filePath) noexcept
		: m_FilePath(filePath.begin(), filePath.end())
	{
		D3D_DXGIDEBUG(*DF::D3DM);

		D3D_THROW(D3DReadFileToBlob(m_FilePath.c_str(), &m_pBlob));
		D3D_THROW(GetDevice()->CreateGeometryShader(m_pBlob->GetBufferPointer(), m_pBlob->GetBufferSize(), nullptr, &m_pGS));
	}

	void GeometryShader::Bind() noexcept
	{
		GetContext()->GSSetShader(m_pGS.Get(), nullptr, NULL);
	}
	void GeometryShader::Unbind() noexcept
	{
		GetContext()->GSSetShader(nullptr, nullptr, NULL);
	}
}