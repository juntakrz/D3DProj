#pragma once

#include "IBind.h"

namespace Bind
{
	class Rasterizer : public IBind
	{
	public:
		Rasterizer(D3D11_CULL_MODE cullMode, D3D11_FILL_MODE fillMode = D3D11_FILL_SOLID) noexcept;

		void Bind() noexcept override;

	private:
		COMPTR<ID3D11RasterizerState> m_pRState;
	};
}