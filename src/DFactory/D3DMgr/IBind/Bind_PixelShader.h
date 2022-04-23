#pragma once

#include "IBind.h"

namespace Bind
{

	class PixelShader : public IBind
	{
	public:
		PixelShader(D3DMgr& d3dMgr, const std::string& filePath) noexcept;
		void Bind(D3DMgr& d3dMgr) noexcept override;

	protected:
		COMPTR<ID3DBlob> m_pBlob;
		COMPTR<ID3D11PixelShader> m_pPS;
		std::wstring m_FilePath;
	};
}