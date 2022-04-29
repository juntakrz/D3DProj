#pragma once

#include "../WndMgr/CException.h"
#include "D3DMgr_Def.h"
#include "dxerr/dxerr.h"
#include "../../imgui/imgui_impl_win32.h"
#include "../../imgui/imgui_impl_dx11.h"
#include "../Util/imGuiMgr.h"
#include "CDXGIDebug.h"
#include "Cameras/CCamera.h"
#include "../DFSurface.h"

namespace Bind
{
	class IBind;
}

class D3DMgr
{
	friend Bind::IBind;
	friend class MeshCore;

	D3D_FEATURE_LEVEL					m_d3dFeatureLvl;
	COMPTR<ID3D11Device>				m_pDevice;
	COMPTR<IDXGISwapChain>				m_pSwap;
	COMPTR<ID3D11DeviceContext>			m_pContext;
	COMPTR<ID3D11RenderTargetView>		m_pRenderTarget;
	COMPTR<ID3D11DepthStencilView>		m_pDSV;
	COMPTR<ID3D11BlendState>			m_pBlendState;

#ifdef _DEBUG
	CDXGIDebug dxgiDebug;
#endif

	HRESULT hr;
	bool m_imguiEnabled = CWND_IMGUIENABLED;
	float m_ClearColor[4] = { 0.0f, 0.0f, 0.0f, 1.0f };
	float m_DefaultViewportWidth = CWND_DEFAULTWIDTH, m_DefaultViewportHeight = CWND_DEFAULTHEIGHT;

	DirectX::XMMATRIX m_ProjectionMatrix =
		DirectX::XMMatrixPerspectiveLH(1.0f, m_DefaultViewportHeight / m_DefaultViewportWidth, 0.5f, 10000.0f);
		//DirectX::XMMatrixOrthographicLH(1.0f, m_DefaultViewportHeight / m_DefaultViewportWidth, 0.5f, 10000.0f);
	class CCamera* m_pCamera;

public:
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

	void SetDefaultViewportSize(const std::pair<uint16_t, uint16_t> viewportSize);

	void BeginFrame(const float red, const float green, const float blue) noexcept;
	void BeginFrame() noexcept;
	void EndFrame();

	void ClearBuffer() noexcept;
	void ClearBuffer(const float red, const float green, const float blue) noexcept;
	void SetClearBufferColor(const float red, const float green, const float blue) noexcept;

	void RenderWireframe(bool enable = false) noexcept;

	void SetProjection(DirectX::FXMMATRIX& projection) noexcept;
	DirectX::XMMATRIX GetProjection() const noexcept;

	void SetCamera(CCamera* pCamera) noexcept;
	CCamera* GetCamera() const noexcept;

	void DrawIndexed(const UINT& count) noexcept;

	ID3D11Device* GetDevice() noexcept;
};