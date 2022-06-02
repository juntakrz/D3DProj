#include "Bind_PixelShader.h"
#include "../../DFactory.h"

namespace Bind
{
	PixelShader::PixelShader(const std::string& filePath) noexcept : m_FilePath(filePath)
	{
		D3D_DXGIDEBUG(*DF::D3DM);

		m_pPS = reinterpret_cast<ID3D11PixelShader*>(DF::Engine->MatM->ShaderAdd(filePath));
	}

	void PixelShader::Bind() noexcept
	{
		GetContext()->PSSetShader(m_pPS, nullptr, NULL);
	}
	void PixelShader::Unbind() noexcept
	{
		GetContext()->PSSetShader(nullptr, nullptr, NULL);
	}
}