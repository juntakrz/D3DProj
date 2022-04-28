#pragma once

#include "IBind.h"

namespace Bind
{

	class InputLayout : public IBind
	{
	public:
		InputLayout(const std::vector<D3D11_INPUT_ELEMENT_DESC>& d3dIEDesc, ID3DBlob* d3dBlob);
		void Bind() noexcept override;

	protected:
		COMPTR<ID3D11InputLayout> pInputLayout;
	};
}