#include "../../pch.h"
#include "WndMgr.h"

WndMgr::WndClass WndMgr::WndClass::wndClass;

WndMgr::WndClass::WndClass() noexcept
	: hInst(GetModuleHandle(nullptr))
{
	WNDCLASSEX wc = { 0 };
	wc.cbSize = sizeof(wc);
	wc.style = CS_OWNDC;
	wc.lpfnWndProc = InitWndProc;			//connecting WndClass object to
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = GetInstance();
	wc.hIcon = static_cast<HICON>(::LoadImageW(hInst, MAKEINTRESOURCEW(IDI_APPICON), IMAGE_ICON, 64, 64, LR_DEFAULTCOLOR));
	wc.hIconSm = static_cast<HICON>(::LoadImageW(hInst, MAKEINTRESOURCEW(IDI_APPICON), IMAGE_ICON, 16, 16, LR_DEFAULTCOLOR));;
	wc.hCursor = nullptr;
	wc.hbrBackground = nullptr;
	wc.lpszMenuName = nullptr;
	wc.lpszClassName = GetName();
	RegisterClassEx(&wc);
}

WndMgr::WndClass::~WndClass()
{
	UnregisterClass(wndClassName, GetInstance());
}

const wchar_t* WndMgr::WndClass::GetName() noexcept
{
	return wndClassName;
}

HINSTANCE WndMgr::WndClass::GetInstance() noexcept
{
	return wndClass.hInst;
}