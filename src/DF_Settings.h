#pragma once

#define COMPTR Microsoft::WRL::ComPtr

#define _XM_AVX2_INTRINSICS_

//default app definitions
#ifndef CWND_WNDNAME
#define CWND_WNDNAME L"Direct3D Window"
#endif

#ifndef CWND_WNDCLASS
#define CWND_WNDCLASS L"D3DRenderer"
#endif

#ifndef CWND_DEFAULTWIDTH
#define CWND_DEFAULTWIDTH 1360
#endif

#ifndef CWND_DEFAULTHEIGHT
#define CWND_DEFAULTHEIGHT 768
#endif

#ifndef CWND_ADJUSTCLIENTAREA
#define ADJCLAREA true
#endif

#ifndef CWND_WNDSTYLE
#define CWND_WNDSTYLE WS_CAPTION | WS_MINIMIZEBOX | WS_SYSMENU
#endif

#ifndef CWND_MSGLOOPFPS
#define CWND_MSGLOOPFPS 144
#endif

#ifndef IDI_APPICON
#define IDI_APPICON 101
#endif

#ifndef CWND_IMGUIENABLED
#define CWND_IMGUIENABLED true
#endif

#define CWND_EXCEPT(hr) WndMgr::Exception(__LINE__,__FILE__,hr)
#define CWND_LASTEXCEPT(hr) WndMgr::Exception(__LINE__,__FILE__,GetLastError())

//assert macro
#define ASSERT(x) if(!(x)) __debugbreak();

#ifdef _CONSOLEAPP
#define LOG(x) std::cout << (x) << "\n";
#else
#define LOG(x)
#endif

#define NOMINMAX

/*
//windows definitions
#ifndef FULLWIN
#define WIN32_LEAN_AND_MEAN
#define NOGDICAPMASKS
#define NOSYSMETRICS
#define NOMENUS
#define NOICONS
#define NOSYSCOMMANDS
#define NORASTEROPS
#define OEMRESOURCE
#define NOATOM
#define NOCLIPBOARD
#define NOCOLOR
#define NOCTLMGR
#define NODRAWTEXT
#define NOKERNEL
#define NONLS
#define NOMEMMGR
#define NOMETAFILE
#define NOMINMAX
#define NOOPENFILE
#define NOSCROLL
#define NOSERVICE
#define NOSOUND
#define NOTEXTMETRIC
#define NOWH
#define NOCOMM
#define NOKANJI
#define NOHELP
#define NOPROFILER
#define NODEFERWINDOWPOS
#define NOMCX
#define NORPC
#define NOPROXYSTUB
#define NOIMAGE
#define NOTAPE
#endif

#define NOMINMAX*/

typedef float float32;
typedef double float64;

constexpr float PI32 = 22.0f / 7.0f;
constexpr double PI64 = 22.0 / 7.0;