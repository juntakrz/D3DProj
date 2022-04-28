#include "../../pch.h"
#include "dxerr/dxerr.h"
#include "D3DMgr.h"
#include "../Util/GfxMath.h"

D3DMgr::D3DMgr(HWND hWnd)
{
	DXGI_SWAP_CHAIN_DESC scd = {};
	scd.BufferDesc.Width = 0;			//try resolution of the window
	scd.BufferDesc.Height = 0;
	scd.BufferDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
	scd.BufferDesc.RefreshRate.Numerator = 0;
	scd.BufferDesc.RefreshRate.Denominator = 0;
	scd.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
	scd.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	scd.SampleDesc.Count = 1;
	scd.SampleDesc.Quality = 0;
	scd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;	//using this as main buffer
	scd.BufferCount = 1;								//amount of additional buffers
	scd.OutputWindow = hWnd;
	scd.Windowed = true;
	scd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;			//buffer swapping style
	scd.Flags = NULL;

	UINT rlFlags = 0u;
#ifdef _DEBUG
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
		&scd,							//returns swap chain descriptor
		&m_pSwap,						//returns swap chain
		&m_pDevice,						//returns device
		&m_d3dFeatureLvl,				//returns current feature level
		&m_pContext
	));

	//get access to back buffer in a swap chain
	COMPTR<ID3D11Resource> pBackBuffer;
	D3D_THROW_INFO(m_pSwap->GetBuffer(0, __uuidof(ID3D11Resource), &pBackBuffer));
	D3D_THROW_INFO(m_pDevice->CreateRenderTargetView(pBackBuffer.Get(), nullptr, &m_pRenderTarget));

	//Z-buffer
	D3D11_DEPTH_STENCIL_DESC dsd = {};
	dsd.DepthEnable = true;
	dsd.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	dsd.DepthFunc = D3D11_COMPARISON_LESS;
	COMPTR<ID3D11DepthStencilState> pDSState;
	D3D_THROW_INFO(m_pDevice->CreateDepthStencilState(&dsd, &pDSState));

	//bind depth state
	D3D_THROW_INFO(m_pContext->OMSetDepthStencilState(pDSState.Get(), 1u));

	//create depth stencil texture
	COMPTR<ID3D11Texture2D> pDepthStencil;

	D3D11_TEXTURE2D_DESC stencilDesc = {};
	stencilDesc.Usage = D3D11_USAGE_DEFAULT;
	stencilDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	stencilDesc.Width = m_DefaultViewportWidth;
	stencilDesc.Height = m_DefaultViewportHeight;
	stencilDesc.MipLevels = 1u;
	stencilDesc.ArraySize = 1u;
	stencilDesc.Format = DXGI_FORMAT_D32_FLOAT;
	stencilDesc.SampleDesc.Count = 1u;
	stencilDesc.SampleDesc.Quality = 0u;

	D3D_THROW_INFO(m_pDevice->CreateTexture2D(&stencilDesc, nullptr, &pDepthStencil));

	//create depth stencil view
	D3D11_DEPTH_STENCIL_VIEW_DESC dsvd = {};
	dsvd.Format = DXGI_FORMAT_D32_FLOAT;
	dsvd.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	dsvd.Texture2D.MipSlice = 0u;
	
	D3D_THROW_INFO(m_pDevice->CreateDepthStencilView(pDepthStencil.Get(), &dsvd, &m_pDSV));

	//bind render target and depth stencil view to OM (output merger)
	m_pContext->OMSetRenderTargets(1u, m_pRenderTarget.GetAddressOf(), m_pDSV.Get());

	//rasterizer test
	COMPTR<ID3D11RasterizerState> pRState;

	D3D11_RASTERIZER_DESC RDesc = {};
	RDesc.FillMode = D3D11_FILL_SOLID;
	RDesc.CullMode = D3D11_CULL_BACK;
	RDesc.FrontCounterClockwise = false;
	
	D3D_THROW_INFO(m_pDevice->CreateRasterizerState(&RDesc, &pRState));
	m_pContext->RSSetState(pRState.Get());

	//configure viewport and rasterizer
	D3D11_VIEWPORT vp = {};
	vp.Width = m_DefaultViewportWidth;
	vp.Height = m_DefaultViewportHeight;
	vp.MinDepth = 0.f;
	vp.MaxDepth = 1.f;
	vp.TopLeftX = 0.f;
	vp.TopLeftY = 0.f;
	m_pContext->RSSetViewports(1u, &vp);	//amount of viewports, pointer to array of viewports

	//initialize imGui here
	imGuiMgr::Get();
	ImGui_ImplDX11_Init(m_pDevice.Get(), m_pContext.Get());
}

D3DMgr::~D3DMgr()
{
	ImGui_ImplDX11_Shutdown();
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

void D3DMgr::SetDefaultViewportSize(const std::pair<uint16_t, uint16_t> viewportSize)
{
	m_DefaultViewportWidth = (float)viewportSize.first;
	m_DefaultViewportHeight = (float)viewportSize.second;
}

void D3DMgr::BeginFrame(const float red, const float green, const float blue) noexcept
{
	if (m_imguiEnabled)
	{
		ImGui_ImplDX11_NewFrame();
		ImGui_ImplWin32_NewFrame();
		ImGui::NewFrame();
	}

	const float color[]{ red, green, blue, 1.0f };
	m_pContext->ClearRenderTargetView(m_pRenderTarget.Get(), color);
	m_pContext->ClearDepthStencilView(m_pDSV.Get(), D3D11_CLEAR_DEPTH, 1.f, 0u);
}

void D3DMgr::BeginFrame() noexcept
{
	if (m_imguiEnabled)
	{
		ImGui_ImplDX11_NewFrame();
		ImGui_ImplWin32_NewFrame();
		ImGui::NewFrame();
	}

	m_pContext->ClearRenderTargetView(m_pRenderTarget.Get(), m_ClearColor);
	m_pContext->ClearDepthStencilView(m_pDSV.Get(), D3D11_CLEAR_DEPTH, 1.f, 0u);
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

void D3DMgr::ClearBuffer() noexcept
{
	m_pContext->ClearRenderTargetView(m_pRenderTarget.Get(), m_ClearColor);
	m_pContext->ClearDepthStencilView(m_pDSV.Get(), D3D11_CLEAR_DEPTH, 1.f, 0u);
}

void D3DMgr::ClearBuffer(const float red, const float green, const float blue) noexcept
{
	const float color[] { red, green, blue, 1.f };
	m_pContext->ClearRenderTargetView(m_pRenderTarget.Get(), color);
	m_pContext->ClearDepthStencilView(m_pDSV.Get(), D3D11_CLEAR_DEPTH, 1.f, 0u);
}

void D3DMgr::SetClearBufferColor(const float red, const float green, const float blue) noexcept
{
	m_ClearColor[0] = red;
	m_ClearColor[1] = green;
	m_ClearColor[2] = blue;
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

void D3DMgr::SetProjection(DirectX::FXMMATRIX& projection) noexcept
{
	m_ProjectionMatrix = projection;
}

DirectX::XMMATRIX D3DMgr::GetProjection() const noexcept
{
	return m_ProjectionMatrix;
}

void D3DMgr::SetCamera(CCamera* pCamera) noexcept
{
	m_pCamera = pCamera;
}

CCamera* D3DMgr::GetCamera() const noexcept
{
	return m_pCamera;
}

void D3DMgr::DrawIndexed(const UINT& count) noexcept
{
	D3D_THROW_INFO(m_pContext->DrawIndexed(count, 0u, 0u));
}

ID3D11Device* D3DMgr::GetDevice() noexcept
{
	return m_pDevice.Get();
}
