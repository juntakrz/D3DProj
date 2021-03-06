#pragma once

#include "IBind.h"

namespace Bind
{

	class PixelShader : public IBind
	{
	public:
		PixelShader(const std::string& filePath) noexcept;
		void Bind() noexcept override;
		void Unbind() noexcept override;

	protected:
		ID3D11PixelShader* m_pPS;
		std::string m_FilePath;
	};
}