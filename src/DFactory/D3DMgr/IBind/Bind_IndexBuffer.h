#pragma once

#include "IBind.h"

namespace Bind
{

	class IndexBuffer : public IBind
	{
	public:
		IndexBuffer(const std::vector<uint32_t>& indices);
		void Bind() noexcept override;
		uint32_t GetCount() const noexcept;

	protected:
		COMPTR<ID3D11Buffer> m_pIndexBuffer;
		uint32_t m_count = 0;
	};
}