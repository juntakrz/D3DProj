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
#include "../Common/DF_Const.h"

class D3DMgr
{
private:
	friend class MeshCore;

	// core
	D3D_FEATURE_LEVEL					m_d3dFeatureLvl;
	COMPTR<IDXGISwapChain>				m_pSwap;
	COMPTR<ID3D11Device>				m_pDevice;
	COMPTR<ID3D11DeviceContext>			m_pContext;

	// render copy shader resource for render-to-texture effects (needs better impl)
	COMPTR<ID3D11Texture2D>				m_pBufCopy_Tex;
	COMPTR<ID3D11ShaderResourceView>	m_pBufCopy_SR;

	// states
	COMPTR<ID3D11DepthStencilState>		m_pDSStates[5];
	COMPTR<ID3D11BlendState>			m_pBlendState;

	//dynamically allocated RTV pointers
	static const uint8_t maxRT = 6;

	struct RenderTarget
	{
		COMPTR<ID3D11Texture2D>				pTex;
		COMPTR<ID3D11RenderTargetView>		pRTV;
		COMPTR<ID3D11ShaderResourceView>	pSRV;
		D3D11_VIEWPORT vp;
		std::string name;
		uint16_t width = 0;
		uint16_t height = 0;
	};

	struct DSTarget
	{
		COMPTR<ID3D11Texture2D>				pDSTex;
		COMPTR<ID3D11DepthStencilView>		pDSV;
		COMPTR<ID3D11ShaderResourceView>	pDepthSRV;
		COMPTR<ID3D11ShaderResourceView>	pStencilSRV;
		std::string name;
		uint16_t width = 0;
		uint16_t height = 0;
		bool isShaderResource = false;
	};

	ID3D11RenderTargetView* nullRTV = nullptr;
	ID3D11DepthStencilView* nullDSV = nullptr;

	HRESULT hr;
	bool m_imguiEnabled = CWND_IMGUIENABLED;
	bool m_ShowDepth = false;
	bool m_IsHDR = false;
	float m_VWidth = CWND_DEFAULTWIDTH, m_VHeight = CWND_DEFAULTHEIGHT;
	float m_AspectRatio = m_VWidth / m_VHeight;

	class CCamera* m_pCamera;

	std::unordered_map<std::string, std::unique_ptr<RenderTarget>> renderTargets;
	std::unordered_map<std::string, std::unique_ptr<DSTarget>> depthTargets;
	std::unordered_map<std::string, std::unique_ptr<RenderSurface>> surfaceTargets;

	// initialize scripts
	void CreateRenderSR(bool isHDR) noexcept;		//create render buffer copy resource
	void CreateStencilStates() noexcept;
	void RTInitBuffers(bool isHDR) noexcept;

public:
#ifdef _DEBUG
	CDXGIDebug dxgiDebug;
#endif
	//initial RTV, DSV, number of RTVs
	std::string m_RTId = "", m_DSId = "";
	uint8_t m_RTNum = 1;

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

public:
	// experimental predicate code
	COMPTR<ID3D11Predicate> m_pPredicate = nullptr;
	D3D11_QUERY_DESC qPred{ D3D11_QUERY_OCCLUSION_PREDICATE, D3D11_QUERY_MISC_PREDICATEHINT };
	//

	D3DMgr(HWND hWnd);
	~D3DMgr();
	D3DMgr(const D3DMgr&) = delete;
	D3DMgr& operator=(const D3DMgr&) = delete;

	void EnableImGui() noexcept;
	void DisableImGui() noexcept;
	bool IsImGuiEnabled() const noexcept;

	void			SetResolution(const uint16_t width, const uint16_t height) noexcept;
	const XMFLOAT2& GetResolution() const noexcept;
	const float&	GetAspectRatio() const noexcept;

	void			SetShowDepth(bool show) noexcept;
	void			SetShowDepth() noexcept;
	const bool&		GetShowDepth() const noexcept;

	void BeginFrame() noexcept;
	void EndFrame();

	//clear buffer only (optionally corresponding depth buffer)
	void Clear(const std::string& renderTarget, const std::string& depthTarget = "") noexcept;
	void Clear(const std::string& renderTarget, const std::string& depthTarget, const XMFLOAT4& color) noexcept;

	// clear depth stencil buffer
	void ClearDSBuffer(const std::string& name) noexcept;

	void SetDepthStencilState(const uint8_t state) noexcept;

	void RenderWireframe(bool enable = false) noexcept;

	//set and get currently active camera
	void SetCamera(CCamera* pCamera) noexcept;
	CCamera* Camera() const noexcept;

	//basic wrapper for Direct3D DrawIndexed method
	void DrawIndexed(const UINT& count) noexcept;

	//access D3D11 device and context directly
	ID3D11Device*			Device() noexcept;
	ID3D11DeviceContext*	Context() noexcept;

	/* * * * * * D3DMgr_RT.cpp * */

	// creates render buffer
	bool	CreateRenderTarget(std::string name, int16_t width = -1, int16_t height = -1, bool isHDR = false) noexcept;

	// creates depth-stencil buffer based on a provided render buffer
	bool	CreateDepthTarget(std::string name, std::string srcTarget, uint8_t SRVMode = DF::DS_SRVMode::None, uint8_t usage = DF::DS_Usage::DepthStencil) noexcept;

	// creates standalone depth-stencil buffer
	bool	CreateDepthTarget(std::string name, int16_t width, int16_t height, uint8_t SRVMode = DF::DS_SRVMode::None, uint8_t usage = DF::DS_Usage::DepthStencil) noexcept;

	// create a copy of a provided buffer
	bool	CreateCompatibleTarget(std::string name, std::string srcTarget, bool isDepthTarget, bool createView) noexcept;

	// binds render target view and depth stencil view (no error prevention for speed, define targets with care)
	void	RTBind(const std::string& renderTarget, const std::string& depthTarget, const uint8_t& num = 1) noexcept;

	// copies render target buffer (no error prevention for speed, define targets with care)
	void	RTCopyTarget(const std::string& srcTarget, const std::string& destTarget, bool isDepthBuffer) noexcept;

	// following methods are not checked for errors, require correct defintions
	RenderTarget*				GetRenderTarget(const std::string& name) noexcept;
	DSTarget*					GetDepthTarget(const std::string& name) noexcept;
	ID3D11RenderTargetView*		GetRTView(const std::string& name) noexcept;
	ID3D11DepthStencilView*		GetDSView(const std::string& name) noexcept;

	//external render targets, RTV and DSV are related by the same index
	void RTSet(ID3D11RenderTargetView* pRTV, ID3D11DepthStencilView* pDSV, uint8_t num = 1) noexcept;
	bool RTRemove(const std::string& name) noexcept;	// deletes render or depth target by its name

	bool RTSetAsShaderResource(const std::string& id, const uint8_t& shaderType, const uint8_t& slot) noexcept;

	/* * * * * * * * * * * * * * */

	/* * * * * * D3DMgr_RS.cpp * */

	// create rendering surface
	bool						CreateRenderSurface(std::string name, float scale = 1.0f) noexcept;

	// access surface targets
	RenderSurface*				Surface(const std::string& name) noexcept;

	// render to surface targets
	void						RenderBufferToSurface(const std::string& rtTarget, const std::string& surface) noexcept;
	void						RenderDepthToSurface(const std::string& dsTarget, const std::string& surface) noexcept;

	/* * * * * * * * * * * * * * */
};