#pragma once

#define D3D_EXCEPT_NOMSG(hr) D3DMgr::HrException(__LINE__, __FILE__, (hr))
#define D3D_THROW_NOMSG(hrcall)  if (FAILED(hr = (hrcall))) throw D3DMgr::HrException(__LINE__, __FILE__, hr)

#ifdef _DEBUG
#define D3D_EXCEPT(hr) D3DMgr::HrException(__LINE__,__FILE__,(hr), dxgiDebug.GetMessages())
#define D3D_THROW(hrcall) dxgiDebug.Set(); if(FAILED(hr = (hrcall))) throw D3D_EXCEPT(hr)
#define D3D_THROW_INFO(call) dxgiDebug.Set(); (call); { auto wstrv = dxgiDebug.GetMessages(); if (!wstrv.empty()) {throw D3DMgr::InfoException(__LINE__, __FILE__, wstrv);}}
#define D3D_DEVICE_REMOVED_EXCEPT(hr) D3DMgr::DeviceRemovedException(__LINE__, __FILE__, (hr), dxgiDebug.GetMessages())
#else
#define D3D_EXCEPT(hr) D3DMgr::HrException(__LINE__,__FILE__,(hr))
#define D3D_THROW(hrcall) D3D_THROW_NOMSG(hrcall)
#define D3D_THROW_INFO(call) (call)
#define D3D_DEVICE_REMOVED_EXCEPT(hr) D3DMgr::DeviceRemovedException(__LINE__, __FILE__, (hr))
#endif

#ifdef _DEBUG
#define D3D_DXGIDEBUG(d3dMgr) HRESULT hr; CDXGIDebug& dxgiDebug = GetDXGIDebug((d3dMgr))
#else
#define D3D_DXGIDEBUG(d3dMgr) HRESULT hr
#endif

#define SAFE_RELEASE(ptr) if(ptr) { ptr->Release(); ptr = nullptr; }