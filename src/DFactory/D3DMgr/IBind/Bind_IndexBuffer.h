#pragma once

#include "IBind.h"

namespace Bind
{

	class IndexBuffer : public IBind
	{
	public:
		IndexBuffer(D3DMgr& d3dMgr, const std::vector<uint32_t>& indices);
		void Bind(D3DMgr& d3dMgr) noexcept override;
		uint32_t GetCount() const noexcept;

	protected:
		COMPTR<ID3D11Buffer> m_pIndexBuffer;
		uint32_t m_count = 0;
	};
}