#pragma once

#include "../WndMgr/CException.h"
#include "../Util/CDXGIDebug.h"
#include "D3DMgr_Def.h"
#include "dxerr/dxerr.h"
#include "../../imgui/imgui_impl_win32.h"
#include "../../imgui/imgui_impl_dx11.h"
#include "../Util/imGuiMgr.h"
#include "../Common/DF_Math.h"
#include "Cameras/CCamera.h"
#include "RenderSurface.h"

namespace Bind
{
	class IBind;
}

class D3DMgr
{
private:
	friend Bind::IBind;
	friend class MeshCore;

	enum StencilState
	{
		stOff, stWrite, stMask
	};

	enum RenderBuffer
	{
		RB_Back		= 0,
		RB_Render	= 1,
		RB_Blur		= 2,
		RB_Resample = 3
	};

	enum DepthStencilBuffer
	{
		DSB_Back		= 0,
		DSB_Render		= 1,
		DSB_Resample	= 2
	};

	// core
	D3D_FEATURE_LEVEL					m_d3dFeatureLvl;
	COMPTR<IDXGISwapChain>				m_pSwap;
	COMPTR<ID3D11Device>				m_pDevice;
	COMPTR<ID3D11DeviceContext>			m_pContext;

	// render copy shader resource
	COMPTR<ID3D11Texture2D>				m_pBufCopy_Tex;
	COMPTR<ID3D11ShaderResourceView>	m_pBufCopy_SR;

	// states
	COMPTR<ID3D11DepthStencilState>		m_pDSStates[3];
	COMPTR<ID3D11BlendState>			m_pBlendState;

	//dynamically allocated RTV pointers
	static const uint8_t maxRT = 6;

	struct RenderTarget
	{
		COMPTR<ID3D11Texture2D>				pTex;
		COMPTR<ID3D11RenderTargetView>		pRTV;
		COMPTR<ID3D11ShaderResourceView>	pSRV;
		D3D11_VIEWPORT vp;
		std::string name = "ERROR";
		uint16_t width = 0;
		uint16_t height = 0;
	};

	struct DSTarget
	{
		COMPTR<ID3D11Texture2D>				pDSTex;
		COMPTR<ID3D11DepthStencilView>		pDSV;
		std::string name = "ERROR";
		uint16_t width = 0;
		uint16_t height = 0;
	};

	struct SurfaceTarget
	{
		std::string name = "ERROR";
		std::unique_ptr<RenderSurface> pRS = nullptr;
	};

	HRESULT hr;
	bool m_imguiEnabled = CWND_IMGUIENABLED;
	bool m_ZBufferEnabled = true;
	bool m_IsHDR = false;
	float m_ClearColor[6][4];
	float m_VWidth = CWND_DEFAULTWIDTH, m_VHeight = CWND_DEFAULTHEIGHT;

	DirectX::XMMATRIX m_ProjectionMatrix =
		DirectX::XMMatrixPerspectiveLH(1.0f, m_VHeight / m_VWidth, 0.001f, 10000.0f);
	//DirectX::XMMatrixOrthographicLH(1.0f, m_DefaultViewportHeight / m_DefaultViewportWidth, 0.5f, 10000.0f);
	class CCamera* m_pCamera;
	std::vector<RenderTarget> renderTargets;
	std::vector<DSTarget> depthTargets;
	std::vector<SurfaceTarget> surfaceTargets;

	// initialize scripts
	void InitClearColors() noexcept;
	void CreateRenderSR(bool isHDR) noexcept;		//create render buffer copy resource
	void CreateStencilStates() noexcept;
	void RTInitBuffers(bool isHDR) noexcept;

public:
#ifdef _DEBUG
	CDXGIDebug dxgiDebug;
#endif
	//initial RTV, number of RTVs
	int m_RTId = 0, m_DSId = 0, m_RTNum = 1;

	class Exception : public CException
	{
		using CException::CException;
	};
	class HrException : public Exception
	{
		HRESULT hr;
		std::string m_ErrorInfo;
	public:
		HrException(int line, const char* file, HRESULT hr, std::vector<std::string> messages = {}) noexcept;
		const char* what() const noexcept override;
		const char* GetType() const noexcept override;
		HRESULT GetErrorCode() const noexcept;
		std::string GetErrorString() const noexcept;
		std::string GetErrorDesc() const noexcept;
		std::string GetErrorInfo() const noexcept;
	};
	class InfoException : public Exception
	{
		std::string m_ErrorInfo;
	public:
		InfoException(int line, const char* file, std::vector<std::string> messages = {}) noexcept;
		const char* what() const noexcept override;
		const char* GetType() const noexcept override;
		std::string GetErrorInfo() const noexcept;
	};
	class DeviceRemovedException : public HrException
	{
		using HrException::HrException;
		std::string reason;
	public:
		const char* GetType() const noexcept;
	};

	D3DMgr(HWND hWnd);
	~D3DMgr();
	D3DMgr(const D3DMgr&) = delete;
	D3DMgr& operator=(const D3DMgr&) = delete;

	void EnableImGui() noexcept;
	void DisableImGui() noexcept;
	bool IsImGuiEnabled() const noexcept;

	void SetViewportSize(const uint16_t width, const uint16_t height) noexcept;

	void BeginFrame() noexcept;
	void EndFrame();

	//clear buffer only (optionally corresponding depth buffer)
	void Clear(uint8_t rtIndex = 0, bool clearDepthBuffer = true, int8_t dsIndex = -1) noexcept;
	void Clear(const float red, const float green, const float blue, uint8_t rtIndex = 0, bool clearDepthBuffer = true, int8_t dsIndex = -1) noexcept;
	void SetClearColor(const float red, const float green, const float blue, uint8_t index = 0) noexcept;

	void SetDepthStencilState(const uint8_t state) noexcept;

	void RenderWireframe(bool enable = false) noexcept;

	void SetProjection(DirectX::FXMMATRIX& projection) noexcept;
	DirectX::XMMATRIX GetProjection() const noexcept;

	//set and get currently active camera
	void SetCamera(CCamera* pCamera) noexcept;
	CCamera* GetCamera() const noexcept;

	//basic wrapper for Direct3D DrawIndexed method
	void DrawIndexed(const UINT& count) noexcept;

	//access D3D11 device and context directly
	ID3D11Device* Device() noexcept;
	ID3D11DeviceContext* Context() noexcept;

	/* * * * * * D3DMgr_RT.cpp * */

	// creates render buffer and returns its new index
	uint8_t	CreateRenderBuffer(int16_t width = -1, int16_t height = -1, bool isHDR = false) noexcept;

	// creates depth-stencil buffer using given render buffer and returns its index
	int8_t	CreateDepthBuffer(uint8_t rtIndex) noexcept;

	// binds render target view and depth stencil view (same if used first overload)
	void	RTBind(uint8_t rtIndex, uint8_t dsIndex, uint8_t num = 1) noexcept;

	// copies render target buffer
	void	RTCopyBuffer(uint8_t indexFrom = 1, uint8_t indexTo = 0) noexcept;

	std::vector<RenderTarget>* RenderTargets() noexcept;
	std::vector<DSTarget>* DepthTargets() noexcept;
	ID3D11RenderTargetView* RTGetRTV(uint8_t index) noexcept;
	ID3D11DepthStencilView* RTGetDSV(uint8_t index) noexcept;

	//external render targets, RTV and DSV are related by the same index
	void RTSet(ID3D11RenderTargetView* pRTV, ID3D11DepthStencilView* pDSV, uint8_t num = 1) noexcept;
	void RTRemove(uint8_t index) noexcept;

	/* * * * * * * * * * * * * * */

	/* * * * * * D3DMgr_RS.cpp * */

	// create render surface
	uint8_t CreateRenderSurface(std::string name, float scale = 1.0f) noexcept;
	std::vector<SurfaceTarget>* SurfaceTargets() noexcept;
	RenderSurface* SurfaceTargets(uint8_t index) noexcept;

	/* * * * * * * * * * * * * * */
};