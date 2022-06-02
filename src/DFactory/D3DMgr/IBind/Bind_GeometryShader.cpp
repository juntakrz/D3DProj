#include "Bind_GeometryShader.h"
#include "../../DFactory.h"

namespace Bind
{
	GeometryShader::GeometryShader(const std::string& filePath) noexcept
		: m_FilePath(filePath)
	{
		D3D_DXGIDEBUG(*DF::D3DM);

		m_pGS = reinterpret_cast<ID3D11GeometryShader*>(DF::Engine->MatM->ShaderAdd(filePath));
	}

	void GeometryShader::Bind() noexcept
	{
		GetContext()->GSSetShader(m_pGS, nullptr, NULL);
	}
	void GeometryShader::Unbind() noexcept
	{
		GetContext()->GSSetShader(nullptr, nullptr, NULL);
	}
}