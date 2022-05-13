#include "D3DMgr.h"

void D3DMgr::CreateStencilStates() noexcept
{
	for (uint8_t i = 0; i < 3; i++)
	{
		D3D11_DEPTH_STENCIL_DESC DSDesc = CD3D11_DEPTH_STENCIL_DESC{ CD3D11_DEFAULT{} };
		switch (i)
		{
		case (uint8_t)DF::DS_Stencil::Write:
		{
			DSDesc.DepthEnable = false;
			DSDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
			DSDesc.StencilEnable = true;
			DSDesc.StencilWriteMask = 0xFF;
			DSDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
			DSDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_REPLACE;
			break;
		}
		case (uint8_t)DF::DS_Stencil::Mask:
		{
			DSDesc.DepthEnable = false;
			DSDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
			DSDesc.StencilEnable = true;
			DSDesc.StencilReadMask = 0xFF;
			DSDesc.FrontFace.StencilFunc = D3D11_COMPARISON_NOT_EQUAL;
			DSDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
			break;
		}
		}

		Device()->CreateDepthStencilState(&DSDesc, &m_pDSStates[i]);
	}
}

void D3DMgr::SetDepthStencilState(const uint8_t state) noexcept
{
	D3D_THROW_INFO(Context()->OMSetDepthStencilState(m_pDSStates[state].Get(), 1u));
}