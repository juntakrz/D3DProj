#include "Bind_Topology.h"

namespace Bind
{
	Topology::Topology(D3D11_PRIMITIVE_TOPOLOGY type) : m_type(type)
	{
		//
	}

	void Topology::Bind() noexcept
	{
		GetContext()->IASetPrimitiveTopology(m_type);
	}
}