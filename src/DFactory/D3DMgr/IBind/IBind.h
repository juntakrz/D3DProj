#pragma once

#include "../D3DMgr.h"
#include "../../Common/DF_A.h"

//generic interface for Direct3D11 bindable objects
namespace Bind
{

	class IBind
	{
	protected:
		static ID3D11Device* GetDevice() noexcept;
		static ID3D11DeviceContext* GetContext() noexcept;
#ifdef _DEBUG
		static CDXGIDebug& GetDXGIDebug(D3DMgr& d3dMgr) noexcept
		{
			return d3dMgr.dxgiDebug;
		}
#endif
		;
		D3D_DXGIDEBUG(*DF::D3DM);

	public:
		virtual void Bind() = 0;
		virtual void Unbind() noexcept {};
		virtual ~IBind() = default;
	};
}