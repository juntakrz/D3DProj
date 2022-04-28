#pragma once

#include "IBind.h"

namespace Bind
{

	class PixelShader : public IBind
	{
	public:
		PixelShader(const std::string& filePath) noexcept;
		void Bind() noexcept override;

	protected:
		COMPTR<ID3DBlob> m_pBlob;
		COMPTR<ID3D11PixelShader> m_pPS;
		std::wstring m_FilePath;
	};
}