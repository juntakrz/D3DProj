#include "GDIPlusMgr.h"

ULONG_PTR GDIPlusMgr::m_token = 0;
int32_t GDIPlusMgr::m_refCount = 0;

GDIPlusMgr::GDIPlusMgr()
{
	if (m_refCount++ == 0)
	{
		Gdiplus::GdiplusStartupInput gdiInput;
		gdiInput.GdiplusVersion = 1;
		gdiInput.DebugEventCallback = NULL;
		gdiInput.SuppressBackgroundThread = FALSE;
		gdiInput.SuppressExternalCodecs = FALSE;

		int sCode = 0;

		if ((sCode = Gdiplus::GdiplusStartup(&m_token, &gdiInput, nullptr)) != Gdiplus::Status::Ok)
		{
			std::wstringstream wss;
			wss << "GDI+ Startup failed. Error code: " << sCode;
			MessageBoxW(nullptr, std::wstring(wss.str()).c_str(), L"GDI+ Error", MB_OK | MB_ICONWARNING);
		}
	}
}

GDIPlusMgr::~GDIPlusMgr()
{
	if (--m_refCount == 0)
	{
		Gdiplus::GdiplusShutdown(m_token);
	}
}
