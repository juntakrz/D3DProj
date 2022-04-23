#pragma once

#include "IBind.h"

namespace Bind
{
	class Topology : public IBind
	{
	public:
		Topology(D3DMgr& d3dMgr, D3D11_PRIMITIVE_TOPOLOGY type);
		void Bind(D3DMgr& d3dMgr) noexcept override;

	protected:
		D3D11_PRIMITIVE_TOPOLOGY m_type;
	};
}