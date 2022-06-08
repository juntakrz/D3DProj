#pragma once

#include "IBind.h"

namespace Bind
{
	class GeometryShader : public IBind
	{
	public:
		GeometryShader(const std::string& filePath) noexcept;
		void Bind() noexcept override;
		void Unbind() noexcept override;

	protected:
		ID3D11GeometryShader* m_pGS;
		std::string m_FilePath;
	};
}