#include "../../pch.h"
#include "D3DMgr.h"

D3DMgr::D3DMgr(HWND hWnd)
{
	//create swap chain description
	DXGI_SWAP_CHAIN_DESC SwapDesc = {};
	SwapDesc.BufferDesc.Width = m_VWidth;
	SwapDesc.BufferDesc.Height = m_VHeight;
	SwapDesc.BufferDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
	SwapDesc.BufferDesc.RefreshRate.Numerator = 0;
	SwapDesc.BufferDesc.RefreshRate.Denominator = 0;
	SwapDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
	SwapDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	SwapDesc.SampleDesc.Count = 1;
	SwapDesc.SampleDesc.Quality = 0;
	SwapDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT | DXGI_USAGE_SHADER_INPUT;
	SwapDesc.BufferCount = 2;								//amount of additional buffers
	SwapDesc.OutputWindow = hWnd;
	SwapDesc.Windowed = true;
	SwapDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL;	//buffer swapping style
	SwapDesc.Flags = NULL;

	UINT rlFlags = 0u;
#ifdef _DEBUG || _DFDEBUG
	rlFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

	//required Direct3D feature levels
	const D3D_FEATURE_LEVEL d3dFeatureLvl[] =
	{
		D3D_FEATURE_LEVEL_11_1,
		D3D_FEATURE_LEVEL_11_0,
	};

	D3D_THROW(D3D11CreateDeviceAndSwapChain(
		nullptr,						//handle to VGA, uses the primary one by default
		D3D_DRIVER_TYPE_HARDWARE,		//driver, using hardware accelerated
		nullptr,						//handle to soft driver, not used
		rlFlags,						//runtime layer flags
		d3dFeatureLvl,					//a handle to D3D feature levels
		(UINT)std::size(d3dFeatureLvl),	//number of elements in the above
		D3D11_SDK_VERSION,
		&SwapDesc,							//returns swap chain descriptor
		&m_pSwap,						//returns swap chain
		&m_pDevice,						//returns device
		&m_d3dFeatureLvl,				//returns current feature level
		&m_pContext
	));

	// initialize render target and depth buffer vectors
	renderTargets.push_back(RenderTarget{ nullptr, nullptr, nullptr });
	depthTargets.push_back(DSTarget{ nullptr, nullptr });

	// store resource resolutions for reference
	renderTargets.back().name = "rtBackBuffer";
	renderTargets.back().width = m_VWidth;
	renderTargets.back().height = m_VHeight;
	depthTargets.back().name = "dsBackBuffer";
	depthTargets.back().width = m_VWidth;
	depthTargets.back().height = m_VHeight;

	// get swap chain buffer and put it into the render target vector
	D3D_THROW_INFO(m_pSwap->GetBuffer(0, __uuidof(ID3D11Texture2D), &renderTargets.back().pTex));

	// create the default render target view and assign it
	D3D_THROW_INFO(m_pDevice->CreateRenderTargetView(
		renderTargets.back().pTex.Get(), nullptr, &renderTargets.back().pRTV)
	);

	// create depth stencil texture
	D3D11_TEXTURE2D_DESC stencilDesc = {};
	stencilDesc.Usage = D3D11_USAGE_DEFAULT;
	stencilDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	stencilDesc.Width = m_VWidth;
	stencilDesc.Height = m_VHeight;
	stencilDesc.MipLevels = 1u;
	stencilDesc.ArraySize = 1u;
	stencilDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	stencilDesc.SampleDesc.Count = 1u;
	stencilDesc.SampleDesc.Quality = 0u;

	D3D_THROW_INFO(m_pDevice->CreateTexture2D(&stencilDesc, nullptr, &depthTargets.back().pDSTex));

	// create depth stencil view
	D3D11_DEPTH_STENCIL_VIEW_DESC dsvd = {};
	dsvd.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	dsvd.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	dsvd.Texture2D.MipSlice = 0u;
	
	D3D_THROW_INFO(m_pDevice->CreateDepthStencilView(
		depthTargets.back().pDSTex.Get(), &dsvd, &depthTargets.back().pDSV)
	);

	// bind initial render target and depth stencil view to OM (output merger)
	m_pContext->OMSetRenderTargets(1u, renderTargets.back().pRTV.GetAddressOf(), depthTargets.back().pDSV.Get());
	m_RTId = 0;		// reference values
	m_DSId = 0;
	m_RTNum = 1;

	//rasterizer test
	COMPTR<ID3D11RasterizerState> pRState;
	
	D3D11_RASTERIZER_DESC RDesc = {};
	RDesc.FillMode = D3D11_FILL_SOLID;
	RDesc.CullMode = D3D11_CULL_BACK;
	RDesc.FrontCounterClockwise = false;
	RDesc.DepthBias = 0;
	RDesc.DepthBiasClamp = 0.0f;
	RDesc.DepthClipEnable = false;
	RDesc.MultisampleEnable = false;
	RDesc.ScissorEnable = false;
	RDesc.SlopeScaledDepthBias = 0.0f;
	
	D3D_THROW_INFO(m_pDevice->CreateRasterizerState(&RDesc, &pRState));
	m_pContext->RSSetState(pRState.Get());
	
	//configure alpha blending
	D3D11_BLEND_DESC blendDesc = {};
	blendDesc.RenderTarget[0].BlendEnable = true;
	blendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
	blendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
	blendDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
	blendDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ONE; //zero
	blendDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].RenderTargetWriteMask = 0x0f;

	D3D_THROW_INFO(m_pDevice->CreateBlendState(&blendDesc, &m_pBlendState));
	m_pContext->OMSetBlendState(m_pBlendState.Get(), NULL, 0xffffffff);
	
	//configure viewport and rasterizer
	D3D11_VIEWPORT vp = {};
	vp.Width = m_VWidth;
	vp.Height = m_VHeight;
	vp.MinDepth = 0.f;
	vp.MaxDepth = 1.f;
	vp.TopLeftX = 0.f;
	vp.TopLeftY = 0.f;
	m_pContext->RSSetViewports(1u, &vp);	//amount of viewports, pointer to array of viewports

	renderTargets.back().vp = std::move(vp);

	// Initializations

	// init 'clear' colors
	InitClearColors();

	// create stencil states
	CreateStencilStates();

	// create render buffer and assign pointers
	RTInitBuffers(m_IsHDR);

	// create copy buffer
	CreateRenderSR(m_IsHDR);

	// initialize imGui here
	imGuiMgr::Get();
	ImGui_ImplDX11_Init(m_pDevice.Get(), m_pContext.Get());
}

D3DMgr::~D3DMgr()
{
	ImGui_ImplDX11_Shutdown();
}

void D3DMgr::InitClearColors() noexcept
{
	for (uint8_t i = 0; i < 6; i++)
	{
		m_ClearColor[i][0] = 0.0f;
		m_ClearColor[i][1] = 0.0f;
		m_ClearColor[i][2] = 0.0f;
		(i == 0) ? m_ClearColor[i][3] = 1.0f : m_ClearColor[i][3] = 0.0f;
	}
}

void D3DMgr::EnableImGui() noexcept
{
	m_imguiEnabled = true;
}

void D3DMgr::DisableImGui() noexcept
{
	m_imguiEnabled = false;
}

bool D3DMgr::IsImGuiEnabled() const noexcept
{
	return m_imguiEnabled;
}

void D3DMgr::SetViewportSize(const uint16_t width, const uint16_t height) noexcept
{
	m_VWidth = (float)width;
	m_VHeight = (float)height;
}

const float& D3DMgr::GetAspectRatio() const noexcept
{
	return m_AspectRatio;
}

void D3DMgr::SetShowDepth(bool show) noexcept
{
	m_ShowDepth = show;
}

void D3DMgr::SetShowDepth() noexcept
{
	(m_ShowDepth) ? SetShowDepth(false) : SetShowDepth(true);
}

const bool& D3DMgr::GetShowDepth() const noexcept
{
	return m_ShowDepth;
}

void D3DMgr::BeginFrame() noexcept
{
	/* NEW FRAME */
	
	// bind render target 1 and depth buffer 1 - primary rendering targets, which will be used on a main surface
	RTBind(DF::RBuffers::Render, DF::DSBuffers::Render, 1u);

	// start new imGui frame
	if (m_imguiEnabled)
	{
		ImGui_ImplDX11_NewFrame();
		ImGui_ImplWin32_NewFrame();
		ImGui::NewFrame();
	}

	// CLEAR FRAME BUFFERS
	// 
	// clear back buffer and its depth buffer
	Clear();

	// clear 'render' buffer and its depth buffer
	Clear(DF::RBuffers::Render, DF::DSBuffers::Render);

	// clear 'blur' buffer only
	Clear(DF::RBuffers::Blur);

	// clear 'downsample' buffers
	Clear(DF::RBuffers::Resample, DF::DSBuffers::Resample);

	// clear dir light depth buffer
	ClearDSBuffer(3u);
}

void D3DMgr::EndFrame()
{
	if (IsImGuiEnabled())
	{
		ImGui::Render();
		ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
	}

#ifdef _DEBUG
	dxgiDebug.Set();
#endif

	if (FAILED(hr = m_pSwap->Present(1u, NULL)))
	{
		if (hr == DXGI_ERROR_DEVICE_REMOVED)
		{
			throw D3D_DEVICE_REMOVED_EXCEPT(m_pDevice->GetDeviceRemovedReason());
		}
		else
		{
			throw D3D_EXCEPT(hr);
		}
	}
}

void D3DMgr::Clear(uint8_t rtIndex, bool clearDepthBuffer, int8_t dsIndex) noexcept
{
	m_pContext->ClearRenderTargetView(renderTargets[rtIndex].pRTV.Get(), m_ClearColor[rtIndex]);
	if (clearDepthBuffer)
	{
		// if depth stencil buffer index is -1 - clear the same index as render target
		(dsIndex < 0) ? dsIndex = rtIndex : 0;
		m_pContext->ClearDepthStencilView(depthTargets[dsIndex].pDSV.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0u);
	}
}

void D3DMgr::Clear(const DF::RBuffers& rtIndex)
{
	Clear((uint8_t)rtIndex, false);
}

void D3DMgr::Clear(const DF::RBuffers& rtIndex, const DF::DSBuffers& dsIndex) noexcept
{
	Clear((uint8_t)rtIndex, true, (uint8_t)dsIndex);
}

void D3DMgr::Clear(const float red, const float green, const float blue, uint8_t rtIndex, bool clearDepthBuffer, int8_t dsIndex) noexcept
{
	const float color[4] = { red, green, blue, 1.0f };

	m_pContext->ClearRenderTargetView(renderTargets[rtIndex].pRTV.Get(), color);
	if (clearDepthBuffer)
	{
		// if depth stencil buffer index is -1 - clear the same index as render target
		(dsIndex < 0) ? dsIndex = rtIndex : 0;
		m_pContext->ClearDepthStencilView(depthTargets[dsIndex].pDSV.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0u);
	}

}

void D3DMgr::SetClearColor(const float red, const float green, const float blue, uint8_t index) noexcept
{
	if (index < maxRT - 1)
	{
		m_ClearColor[index][0] = red;
		m_ClearColor[index][1] = green;
		m_ClearColor[index][2] = blue;
		m_ClearColor[index][3] = 1.0f;
	}
}

void D3DMgr::ClearDSBuffer(uint8_t index) noexcept
{
	(index > -1 && index < depthTargets.size() - 1)
		? m_pContext->ClearDepthStencilView(depthTargets[index].pDSV.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0u)
		: void();
}

void D3DMgr::RenderWireframe(bool enable) noexcept
{
	COMPTR<ID3D11RasterizerState> pRState;

	D3D11_RASTERIZER_DESC RDesc{};
	RDesc.CullMode = D3D11_CULL_BACK;

	switch (enable)
	{
	case true:
	{
		RDesc.FillMode = D3D11_FILL_WIREFRAME;
		break;
	}
	case false:
	{
		RDesc.FillMode = D3D11_FILL_SOLID;
		break;
	}
	}

	D3D_THROW_INFO(m_pDevice->CreateRasterizerState(&RDesc, &pRState));
	m_pContext->RSSetState(pRState.Get());
}

void D3DMgr::SetCamera(CCamera* pCamera) noexcept
{
	m_pCamera = pCamera;
}

CCamera* D3DMgr::Camera() const noexcept
{
	return m_pCamera;
}

void D3DMgr::DrawIndexed(const UINT& count) noexcept
{
	D3D_THROW_INFO(m_pContext->DrawIndexed(count, 0u, 0u));
}

ID3D11Device* D3DMgr::Device() noexcept
{
	return m_pDevice.Get();
}

ID3D11DeviceContext* D3DMgr::Context() noexcept
{
	return m_pContext.Get();
}