#include "../../pch.h"
#include "WndMgr.h"

LRESULT WINAPI WndMgr::InitWndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	//catching the message before WM_CREATE
	if (msg == WM_NCCREATE)
	{
		//LPARAM will be a ptr to CREATESTRUCT of the window being created
		const CREATESTRUCTW* const pCreateStruct = reinterpret_cast<CREATESTRUCTW*>(lParam);
		//getting a ptr to a WndMgr object from it
		WndMgr* pWnd = static_cast<WndMgr*>(pCreateStruct->lpCreateParams); //it's 'this' ptr from WndMgr constructor
		//store ptr to the WndMgr object inside the WinAPI-managed window user data
		SetWindowLongPtr(hWnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(pWnd));
		//change window procedure pointer so the window relays its messages to object's procedure
		SetWindowLongPtr(hWnd, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(&RelayWndProc));

		return pWnd->WndProc(hWnd, msg, wParam, lParam);
	}
	return DefWindowProc(hWnd, msg, wParam, lParam);
}

LRESULT WINAPI WndMgr::RelayWndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	//relay messages to object's procedure
	WndMgr* pWnd = reinterpret_cast<WndMgr*>(GetWindowLongPtr(hWnd, GWLP_USERDATA));
	return pWnd->WndProc(hWnd, msg, wParam, lParam);
}

LRESULT CALLBACK WndMgr::WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{

	if (ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam))
	{
		return true;
	}

	switch (msg)
	{
	case WM_CLOSE:
	{
		PostQuitMessage(0);
		return 0;
	}
	case WM_KILLFOCUS:			//window loses focus - clear its key queues
	{
		kbd.ClearKeyStates();
		ShowCursor(true);
		mouse.m_ShowCursor = true;
		ClipCursor(NULL);
		break;
	}
	case WM_SETFOCUS:
	{
		!mouse.IsCursorOn() ? LockMouse() : void();
		break;
	}
	case WM_DISPLAYCHANGE:
	case WM_SIZE:
	{
		if (pD3DMgr) {
			pD3DMgr->m_pSwap->ResizeBuffers(2u, pD3DMgr->GetResolution().x, pD3DMgr->GetResolution().y, DXGI_FORMAT_UNKNOWN, NULL);
		}
		break;
	}

	//CKbd.h KEYBOARD MESSAGES
	case WM_KEYDOWN:
	case WM_SYSKEYDOWN:
	{
		if (!(lParam & 0x40000000) || kbd.IsAutoRepeatEnabled())
		{
			kbd.OnKeyPress(static_cast<uint8_t>(wParam));
		}
		break;
	}
	case WM_KEYUP:
	case WM_SYSKEYUP:
	{
		kbd.OnKeyRelease(static_cast<uint8_t>(wParam));
		break;
	}
	case WM_CHAR:
	{
		kbd.OnGetChar(static_cast<wchar_t>(wParam));
		break;
	}
	//!CKbd.h

	//CMouse.h MOUSE MESSAGES
	case WM_MOUSEMOVE:
	{
		//WARNING! As of right now this message handling code messes with multi-window setup

		const POINTS mpt = MAKEPOINTS(lParam);

		//when inside the client region
		if (mpt.x > -1 && mpt.x < m_width && mpt.y > -1 && mpt.y < m_height)
		{
			mouse.OnMove(mpt.x, mpt.y);

			if (!mouse.IsInsideWnd())
			{
				SetCapture(hWnd);
				!mouse.IsCursorOn() ? LockMouse() : void();
				mouse.OnEnter();
			}
		}
		//when outside the client region
		else
		{
			//and getting button-down messages for when e.g. interacting with the window border
			if (wParam & (VK_LBUTTON | VK_RBUTTON | VK_MBUTTON)) //checking bitmask for mouse buttons against WPARAM
			{
				mouse.OnMove(mpt.x, mpt.y);
			}
			//and not doing anything, release the capture
			else
			{
				ReleaseCapture();
				mouse.OnLeave();
			}
		}
		break;
	}
	case WM_LBUTTONDOWN:
	{
		mouse.OnLPress();
		break;
	}
	case WM_LBUTTONUP:
	{
		mouse.OnLRelease();
		break;
	}
	case WM_RBUTTONDOWN:
	{
		mouse.OnRPress();
		break;
	}
	case WM_RBUTTONUP:
	{
		mouse.OnRRelease();
		break;
	}
	case WM_MBUTTONDOWN:
	{
		mouse.OnMPress();
		break;
	}
	case WM_MBUTTONUP:
	{
		mouse.OnMRelease();
		break;
	}
	case WM_MOUSEWHEEL:
	{
		mouse.OnWheelDelta(GET_WHEEL_DELTA_WPARAM(wParam));
		break;
	}
	//!CMouse.h

	}
	return DefWindowProc(hWnd, msg, wParam, lParam);
}