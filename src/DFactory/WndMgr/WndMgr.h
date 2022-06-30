#pragma once

#include "../../DF_Settings.h"
#include "../../include/imgui/imgui_impl_win32.h"
#include "CException.h"
#include "CKbd.h"
#include "CMouse.h"
#include "../D3DMgr/D3DMgr.h"

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

class WndMgr
{
private:
	class WndClass
	{
	public:
		static const wchar_t* GetName() noexcept;
		static HINSTANCE GetInstance() noexcept;
	private:
		WndClass() noexcept;
		~WndClass() noexcept;
		WndClass(const WndClass&) = delete; //prevent copying
		WndClass operator=(const WndClass&) = delete;
		static constexpr const wchar_t* wndClassName = CWND_WNDCLASS;
		static WndClass wndClass;
		HINSTANCE hInst;
	};

public:
	//WndMgrExcept.cpp
	class Exception : public CException
	{
		HRESULT m_hr;
	public:
		Exception(int line, const char* file, HRESULT hr) noexcept;
		const char* what() const noexcept override;
		const char* GetType() const noexcept override;
		static std::string TranslateErrorCode(HRESULT hr) noexcept;
		HRESULT GetErrorCode() const noexcept;
		std::string GetErrorString() const noexcept;
	};

	WndMgr(uint16_t width, uint16_t height, const wchar_t* name = nullptr);
	WndMgr(uint16_t width, uint16_t height, uint16_t posX, uint16_t posY, const wchar_t* name = nullptr);
	~WndMgr();
	WndMgr(const WndMgr&) = delete;
	WndMgr operator=(const WndMgr&) = delete;

	CKbd kbd;
	CMouse mouse;

	void SetTitle(const wchar_t* title);
	std::pair<uint16_t, uint16_t> GetWindowSize() const noexcept;
	static std::optional<int32_t> PeekMessages(MSG& msg);
	static int32_t GetMessages();
	void LockMouse() noexcept;
	D3DMgr& D3D();

private:
	std::unique_ptr<D3DMgr> pD3DMgr;		//for deferred creation of D3DMgr object since it requires HWND

	uint16_t m_posX, m_posY, m_width, m_height;
	//static uint8_t m_PeekMsgDelay;
	static constexpr uint8_t m_PeekMsgDelay = 1000 / CWND_MSGLOOPFPS / 2;
	HWND m_hWnd;

	void GetDesktopResolution(uint16_t& outWidth, uint16_t& outHeight);
	RECT AdjustWindow(const uint16_t& width, const uint16_t& height);

	//WndMgr_Msg.cpp
	//WINAPI needs static window procedures to connect to object member window procedure
	static LRESULT WINAPI InitWndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
	static LRESULT WINAPI RelayWndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
	LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
};