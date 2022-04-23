#include "Bind_Topology.h"

namespace Bind
{
	Topology::Topology(D3DMgr& d3dMgr, D3D11_PRIMITIVE_TOPOLOGY type) : m_type(type)
	{
		//
	}

	void Topology::Bind(D3DMgr& d3dMgr) noexcept
	{
		GetContext(d3dMgr)->IASetPrimitiveTopology(m_type);
	}
}