#include "Bind_InputLayout.h"

namespace Bind
{

	InputLayout::InputLayout(const std::vector<D3D11_INPUT_ELEMENT_DESC>& d3dIEDesc, ID3DBlob* pVSBytecode)
	{
		D3D_DXGIDEBUG(*DF::pD3DM);

		//creating D3D input layout
		D3D_THROW_INFO(GetDevice()->CreateInputLayout(
			d3dIEDesc.data(),
			(UINT)std::size(d3dIEDesc),
			pVSBytecode->GetBufferPointer(),
			pVSBytecode->GetBufferSize(),
			&pInputLayout
		));
	}

	void InputLayout::Bind() noexcept
	{
		GetContext()->IASetInputLayout(pInputLayout.Get());
	}
}