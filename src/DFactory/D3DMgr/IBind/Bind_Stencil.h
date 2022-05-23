#pragma once

#include "IBind.h"

namespace Bind
{
	/*
	class Stencil : public IBind
	{
		enum class Mode
		{
			Off, Write, Mask
		};
	public:
		Stencil(uint8_t mode)
		{
			//default stencil description setting
			D3D11_DEPTH_STENCIL_DESC DSDesc = CD3D11_DEPTH_STENCIL_DESC{ CD3D11_DEFAULT{} };

			DSDesc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;

			switch (mode)
			{
			case (uint8_t)Mode::Write :
			{
				DSDesc.DepthEnable = false;
				DSDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
				DSDesc.StencilEnable = true;
				DSDesc.StencilWriteMask = 0xFF;
				DSDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
				DSDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_REPLACE;
				break;
			}
			case (uint8_t)Mode::Mask :
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

			D3D_THROW(DF::Device()->CreateDepthStencilState(&DSDesc, &m_pDSS));
		}

		void Bind() noexcept override
		{
			DF::Context()->OMSetDepthStencilState(m_pDSS.Get(), 0xFFu);
		}

	protected:
		COMPTR<ID3D11DepthStencilState> m_pDSS;
	};
	*/
}