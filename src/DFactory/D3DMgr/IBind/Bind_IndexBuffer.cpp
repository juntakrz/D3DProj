#include "Bind_IndexBuffer.h"

namespace Bind
{

	IndexBuffer::IndexBuffer(D3DMgr& d3dMgr, const std::vector<uint32_t>& indices) : m_count((UINT)indices.size())
	{

		D3D_DXGIDEBUG(d3dMgr);

		//descriptors
		D3D11_BUFFER_DESC bd = {};
		bd.Usage = D3D11_USAGE_DEFAULT;
		bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
		bd.CPUAccessFlags = NULL;
		bd.MiscFlags = NULL;
		bd.ByteWidth = UINT(m_count * sizeof(uint32_t));
		bd.StructureByteStride = sizeof(uint32_t);

		D3D11_SUBRESOURCE_DATA sd = {};
		sd.pSysMem = indices.data();

		//create buffer
		D3D_THROW(GetDevice(d3dMgr)->CreateBuffer(&bd, &sd, &m_pIndexBuffer));
	}

	void IndexBuffer::Bind(D3DMgr& d3dMgr) noexcept
	{
		GetContext(d3dMgr)->IASetIndexBuffer(m_pIndexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0u);
	}

	uint32_t IndexBuffer::GetCount() const noexcept
	{
		return m_count;
	}
}