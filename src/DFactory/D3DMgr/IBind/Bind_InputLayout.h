#pragma once

#include "IBind.h"

namespace Bind
{

	class InputLayout : public IBind
	{
	public:
		InputLayout(D3DMgr& d3dMgr, const std::vector<D3D11_INPUT_ELEMENT_DESC>& d3dIEDesc, ID3DBlob* d3dBlob);
		void Bind(D3DMgr& d3dMgr) noexcept override;

	protected:
		COMPTR<ID3D11InputLayout> pInputLayout;
	};
}