#include "../../pch.h"
#include "WndMgr.h"

//PUBLIC

WndMgr::WndMgr(uint16_t width, uint16_t height, const wchar_t* name) : m_width(width), m_height(height)
{
	//if width and/or height are NULL - get them from desktop resolution
	if (width == NULL || height == NULL)
	{
		width = CWND_DEFAULTWIDTH;
		height = CWND_DEFAULTHEIGHT;
	}

	RECT wr = AdjustWindow(width, height);
	m_posX = wr.left;
	m_posY = wr.top;
	m_hWnd = CreateWindowExW(0, WndClass::GetName(), name ? name : CWND_WNDNAME, CWND_WNDSTYLE,
		m_posX, m_posY, wr.right, wr.bottom, nullptr, nullptr, WndClass::GetInstance(), this); //this - a ptr to this class object sent to the created window
	ShowWindow(m_hWnd, SW_SHOWDEFAULT);

	pD3DMgr = std::make_unique<D3DMgr>(m_hWnd);

	if (!m_hWnd)
	{
		throw CWND_LASTEXCEPT();
	}

	mouse.m_hWnd = m_hWnd;

	//initializing imGui for Win32
	ImGui_ImplWin32_Init(m_hWnd);
}

WndMgr::WndMgr(uint16_t width, uint16_t height, uint16_t posX, uint16_t posY, const wchar_t* name) : m_width(width), m_height(height)
{
	//if width and/or height are NULL - get them from desktop resolution
	if (width == NULL || height == NULL)
	{
		width = CWND_DEFAULTWIDTH;
		height = CWND_DEFAULTHEIGHT;
	}

	//would closing this window close the whole program

	RECT wr = AdjustWindow(width, height);
	m_posX = posX;
	m_posY = posY;
	m_hWnd = CreateWindowExW(0, WndClass::GetName(), name ? name : CWND_WNDNAME, CWND_WNDSTYLE,
		m_posX, m_posY, wr.right, wr.bottom, nullptr, nullptr, WndClass::GetInstance(), this); //this - a ptr to this class object sent to the created window
	ShowWindow(m_hWnd, SW_SHOWDEFAULT);

	pD3DMgr = std::make_unique<D3DMgr>(m_hWnd);

	if (!m_hWnd)
	{
		throw CWND_LASTEXCEPT();
	}

	mouse.m_hWnd = m_hWnd;

	//initializing imGui for Win32
	ImGui_ImplWin32_Init(m_hWnd);
}

WndMgr::~WndMgr()
{
	ImGui_ImplWin32_Shutdown();
	DestroyWindow(m_hWnd);
}

void WndMgr::SetTitle(const wchar_t* title)
{
	if (!SetWindowTextW(m_hWnd, title))
	{
		throw CWND_LASTEXCEPT();
	}
}

std::pair<uint16_t, uint16_t> WndMgr::GetWindowSize() const noexcept
{
	return { m_width, m_height };
}

std::optional<int32_t> WndMgr::PeekMessages(MSG& msg)
{
	std::this_thread::sleep_for(std::chrono::milliseconds(m_PeekMsgDelay));
	while (PeekMessageW(&msg, nullptr, 0, 0, PM_REMOVE))
	{
		if (msg.message == WM_QUIT)
		{
			return msg.wParam;
		}

		TranslateMessage(&msg);
		DispatchMessageW(&msg);
	}
	return {};
}

int32_t WndMgr::GetMessages()
{
	MSG msg;
	bool getResult;

	while ((getResult = GetMessageW(&msg, NULL, 0, 0)) > 0)
	{
		TranslateMessage(&msg);
		DispatchMessageW(&msg);
	}

	if (getResult == -1)
	{
		return -1;
	}

	return msg.wParam;
}

void WndMgr::LockMouse() noexcept
{
	RECT r;
	GetClientRect(m_hWnd, &r);
	MapWindowPoints(m_hWnd, HWND_DESKTOP, reinterpret_cast<LPPOINT>(&r), 2);
	ClipCursor(&r);
}

D3DMgr& WndMgr::D3D()
{
	if (!pD3DMgr)
	{
		MessageBoxA(nullptr, "Couldn't initialize Direct3D API", "ERROR", MB_ICONERROR | MB_OK);
	}
	return *pD3DMgr;
}

//PRIVATE

void WndMgr::GetDesktopResolution(uint16_t& outWidth, uint16_t& outHeight)
{
	RECT desktop;
	const HWND hDesktop = GetDesktopWindow();
	GetWindowRect(hDesktop, &desktop);

	outWidth = desktop.right;
	outHeight = desktop.bottom;
}

RECT WndMgr::AdjustWindow(const uint16_t& width, const uint16_t& height)
{
	bool adjustClientArea = ADJCLAREA;
	RECT rFullWin = { 0, 0, width, height };
	if (adjustClientArea)
	{
		if (!AdjustWindowRectEx(&rFullWin, CWND_WNDSTYLE, false, 0))
		{
			throw CWND_LASTEXCEPT();
		};
	}
	uint16_t initPosX = 0;
	uint16_t initPosY = 0;
	uint16_t winResX = rFullWin.right - rFullWin.left;
	uint16_t winResY = rFullWin.bottom - rFullWin.top;

	GetDesktopResolution(initPosX, initPosY);
	initPosX = (initPosX / 2) - (winResX / 2);
	initPosY = (initPosY / 2) - (winResY / 2);

	return { initPosX, initPosY, winResX, winResY };
}