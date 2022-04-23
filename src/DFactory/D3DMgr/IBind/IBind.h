#pragma once

#include "../D3DMgr.h"

//generic interface for Direct3D11 bindable objects

namespace Bind
{

	class IBind
	{
	protected:
		static ID3D11Device* GetDevice(D3DMgr& d3dMgr) noexcept;
		static ID3D11DeviceContext* GetContext(D3DMgr& d3dMgr) noexcept;
#ifdef _DEBUG
		static CDXGIDebug& GetDXGIDebug(D3DMgr& d3dMgr) noexcept
		{
			return d3dMgr.dxgiDebug;
		}
#endif
		;

	public:
		virtual void Bind(D3DMgr& d3dMgr) = 0;
		virtual ~IBind() = default;
	};
}