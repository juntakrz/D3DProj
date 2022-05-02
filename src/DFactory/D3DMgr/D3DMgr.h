#pragma once

#include "../WndMgr/CException.h"
#include "CDXGIDebug.h"
#include "D3DMgr_Def.h"
#include "dxerr/dxerr.h"
#include "../../imgui/imgui_impl_win32.h"
#include "../../imgui/imgui_impl_dx11.h"
#include "../Util/imGuiMgr.h"
#include "../Common/DF_Math.h"
#include "Cameras/CCamera.h"

namespace Bind
{
	class IBind;
}

class D3DMgr
{
private:
	friend Bind::IBind;
	friend class MeshCore;

	D3D_FEATURE_LEVEL					m_d3dFeatureLvl;
	COMPTR<ID3D11Device>				m_pDevice;
	COMPTR<IDXGISwapChain>				m_pSwap;
	COMPTR<ID3D11DeviceContext>			m_pContext;
	COMPTR<ID3D11Texture2D>				m_pBackBuffer;
	COMPTR<ID3D11Texture2D>				m_pAuxRTTex;
	COMPTR<ID3D11RenderTargetView>		m_pRTVMain;
	COMPTR<ID3D11DepthStencilView>		m_pDSVMain;
	COMPTR<ID3D11ShaderResourceView>	m_pSRVMain;
	COMPTR<ID3D11RenderTargetView>		m_pRTVAux;
	COMPTR<ID3D11ShaderResourceView>	m_pSRVAux;
	COMPTR<ID3D11DepthStencilState>		m_pDSStateOn;
	COMPTR<ID3D11DepthStencilState>		m_pDSStateOff;
	COMPTR<ID3D11BlendState>			m_pBlendState;

	//dynamically allocated RTV pointers
	struct RenderTargets
	{
		static const uint8_t maxRTVs = 6;
		//reserved: 0 = back buffer, 1 = primary render to texture resource
		ID3D11RenderTargetView* m_pRTViews[maxRTVs] = { nullptr, nullptr, nullptr, nullptr, nullptr, nullptr };
		ID3D11DepthStencilView* m_pDSViews[maxRTVs] = { nullptr, nullptr, nullptr, nullptr, nullptr, nullptr };
		ID3D11ShaderResourceView* m_pSRViews[maxRTVs] = { nullptr, nullptr, nullptr, nullptr, nullptr, nullptr };
	} RTV;

	HRESULT hr;
	bool m_imguiEnabled = CWND_IMGUIENABLED;
	bool m_ZBufferEnabled = true;
	float m_ClearColor[6][4] = { 0.0f, 0.0f, 0.0f, 1.0f };
	float m_VWidth = CWND_DEFAULTWIDTH, m_VHeight = CWND_DEFAULTHEIGHT;

	DirectX::XMMATRIX m_ProjectionMatrix =
		DirectX::XMMatrixPerspectiveLH(1.0f, m_VHeight / m_VWidth, 0.001f, 10000.0f);
	//DirectX::XMMatrixOrthographicLH(1.0f, m_DefaultViewportHeight / m_DefaultViewportWidth, 0.5f, 10000.0f);
	class CCamera* m_pCamera;

	void CreateAuxRenderTarget() noexcept;
	void AssignInitPtrs() noexcept;

public:
#ifdef _DEBUG
	CDXGIDebug dxgiDebug;
#endif

	//initial RTV, number of RTVs
	int m_RTId = 0, m_RTNum = 1;

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

	void BeginFrame(const float red, const float green, const float blue) noexcept;
	void BeginFrame(bool clear = true) noexcept;
	void EndFrame();

	//clear buffer only (optionally corresponding depth buffer)
	void Clear(uint8_t index = 0, bool clearDepthBuffer = true) noexcept;
	void Clear(const float red, const float green, const float blue, uint8_t index = 0, bool clearDepthBuffer = true) noexcept;
	void SetClearColor(const float red, const float green, const float blue, uint8_t index = 0) noexcept;

	void EnableDepthBuffer() noexcept;
	void DisableDepthBuffer() noexcept;

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

	//render targer view methods									//render target id, quantity
	void RTBindMainAndAux() noexcept;								//0, 2
	void RTBindMain() noexcept;										//0, 1
	void RTBindAux() noexcept;										//1, 1
	void RTBind(uint8_t index = 255, uint8_t num = 255) noexcept;	//initial, num

	void RTCopyBuffer() noexcept;

	template<typename T, typename... RTVs>
	void RTSet(T pRTV, RTVs... pRTVs) noexcept
	{
		//std::vector
	}

	//access render targets directly
	RenderTargets& RTGet() noexcept;
	ID3D11RenderTargetView* RTGetRTV(uint8_t index) noexcept;
	ID3D11DepthStencilView* RTGetDSV(uint8_t index) noexcept;

	//external render targets, RTV and DSV are related by the same index
	void RTAdd(uint8_t index, ID3D11RenderTargetView* pRTV, ID3D11DepthStencilView* pDSV) noexcept;
	void RTAddRTV(uint8_t index, ID3D11RenderTargetView* pRTV) noexcept;
	void RTAddDSV(uint8_t index, ID3D11DepthStencilView* pDSV) noexcept;

	void RTRemove(uint8_t index, bool free = false) noexcept;
	void RTRemoveRTV(uint8_t index, bool free = false) noexcept;
	void RTRemoveDSV(uint8_t index, bool free = false) noexcept;

	/* * * * * * * * * * * * * * */
};