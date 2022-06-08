#include "D3DMgr.h"

void D3DMgr::CreateStencilStates() noexcept
{
	for (uint8_t i = 0; i < 5; i++)
	{
		D3D11_DEPTH_STENCIL_DESC DSDesc = CD3D11_DEPTH_STENCIL_DESC{ CD3D11_DEFAULT{} };

		DSDesc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;

		switch (i)
		{
		case DF::DS_Mode::DOn_SWrite:
		{
			DSDesc.StencilEnable = true;
			DSDesc.StencilWriteMask = 0xFF;
			DSDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
			DSDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_REPLACE;
			break;
		}
		case DF::DS_Mode::DOff_SWrite:
		{
			DSDesc.DepthEnable = false;
			DSDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
			DSDesc.StencilEnable = true;
			DSDesc.StencilWriteMask = 0xFF;
			DSDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
			DSDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_REPLACE;
			break;
		}
		case DF::DS_Mode::DOff_SRead:
		{
			DSDesc.DepthEnable = false;
			DSDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
			DSDesc.StencilEnable = true;
			DSDesc.StencilReadMask = 0xFF;
			DSDesc.FrontFace.StencilFunc = D3D11_COMPARISON_NOT_EQUAL;
			DSDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
			break;
		}
		case DF::DS_Mode::DOff_SOff:
		{
			DSDesc.DepthEnable = false;
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