#include "Bind_Rasterizer.h"

Bind::Rasterizer::Rasterizer(D3D11_CULL_MODE cullMode, D3D11_FILL_MODE fillMode) noexcept
{
	D3D_DXGIDEBUG(*DF::D3DM);

	D3D11_RASTERIZER_DESC RDesc = {};
	RDesc.FillMode = fillMode;
	RDesc.CullMode = cullMode;
	RDesc.FrontCounterClockwise = false;
	RDesc.DepthBias = 0;
	RDesc.DepthBiasClamp = 0.0f;
	RDesc.DepthClipEnable = false;
	RDesc.MultisampleEnable = false;
	RDesc.ScissorEnable = false;
	RDesc.SlopeScaledDepthBias = 0.0f;

	D3D_THROW(GetDevice()->CreateRasterizerState(&RDesc, &pRState));
}

void Bind::Rasterizer::Bind() noexcept
{
	GetContext()->RSSetState(pRState.Get());
}
