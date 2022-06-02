#pragma once

#include "IBind.h"

namespace Bind
{
	class VertexShader : public IBind
	{
	public:
		VertexShader(const std::string& filePath) noexcept;
		void Bind() noexcept override;
		ID3DBlob* GetByteCode() const noexcept;

	protected:
		ID3D11VertexShader* m_pVS;
		std::string m_FilePath;
	};
}