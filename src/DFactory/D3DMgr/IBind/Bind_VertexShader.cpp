#include "Bind_VertexShader.h"
#include "../../DFactory.h"

namespace Bind
{
	VertexShader::VertexShader(const std::string& filePath) noexcept : m_FilePath(filePath)
	{
		D3D_DXGIDEBUG(*DF::D3DM);

		m_pVS = reinterpret_cast<ID3D11VertexShader*>(DF::Engine->MatM->ShaderAdd(filePath));
	}

	void VertexShader::Bind() noexcept
	{
		GetContext()->VSSetShader(m_pVS, nullptr, NULL);
	}

	ID3DBlob* VertexShader::GetByteCode() const noexcept
	{
		return DF::Engine->MatM->ShaderByteCode(m_FilePath);
	}
}