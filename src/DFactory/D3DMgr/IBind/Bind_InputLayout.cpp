#include "Bind_InputLayout.h"

namespace Bind
{

	InputLayout::InputLayout(D3DMgr& d3dMgr, const std::vector<D3D11_INPUT_ELEMENT_DESC>& d3dIEDesc, ID3DBlob* pVSBytecode)
	{
		D3D_DXGIDEBUG(d3dMgr);

		//creating D3D input layout
		D3D_THROW_INFO(GetDevice(d3dMgr)->CreateInputLayout(
			d3dIEDesc.data(),
			(UINT)std::size(d3dIEDesc),
			pVSBytecode->GetBufferPointer(),
			pVSBytecode->GetBufferSize(),
			&pInputLayout
		));
	}

	void InputLayout::Bind(D3DMgr& d3dMgr) noexcept
	{
		GetContext(d3dMgr)->IASetInputLayout(pInputLayout.Get());
	}
}