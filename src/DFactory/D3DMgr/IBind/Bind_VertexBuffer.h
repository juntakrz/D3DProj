#pragma once

#include "IBind.h"
#include "../../Util/Vertex.h"

namespace Bind
{
	class VertexBuffer : public IBind
	{
	public:
		template<typename V>
		VertexBuffer(const std::vector<V>& vertices) : m_stride(sizeof(V))
		{
			D3D_DXGIDEBUG(*DFData::pD3DM);

			D3D11_BUFFER_DESC bd = {};
			bd.Usage = D3D11_USAGE_DEFAULT;				//GPU read and write
			bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;	//it's a vertex buffer
			bd.CPUAccessFlags = NULL;					//no CPU access needed
			bd.MiscFlags = NULL;
			bd.ByteWidth = UINT(sizeof(V) * vertices.size());
			bd.StructureByteStride = sizeof(V);

			D3D11_SUBRESOURCE_DATA sd = {};
			sd.pSysMem = vertices.data();				//pointer to the resource memory data read according to the above settings

			//takes buffer and subresource descriptors and returns a ptr to the created buffer
			D3D_THROW(GetDevice()->CreateBuffer(&bd, &sd, &m_pVertexBuffer));
		}

		VertexBuffer(const Vrtx::Buffer& vertices) : m_stride((UINT)vertices.GetLayout().Size())
		{
			D3D_DXGIDEBUG(*DFData::pD3DM);

			D3D11_BUFFER_DESC bd = {};
			bd.Usage = D3D11_USAGE_DEFAULT;				//GPU read and write
			bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;	//it's a vertex buffer
			bd.CPUAccessFlags = NULL;					//no CPU access needed
			bd.MiscFlags = NULL;
			bd.ByteWidth = (UINT)vertices.SizeBytes();
			bd.StructureByteStride = m_stride;

			D3D11_SUBRESOURCE_DATA sd = {};
			sd.pSysMem = vertices.GetData();			//pointer to the resource memory data read according to the above settings

			//takes buffer and subresource descriptors and returns a ptr to the created buffer
			D3D_THROW(GetDevice()->CreateBuffer(&bd, &sd, &m_pVertexBuffer));
		}

		void Bind() noexcept override
		{
			const UINT offset = 0u;						//offset to the first vertex array element
			GetContext()->IASetVertexBuffers(0u, 1u, m_pVertexBuffer.GetAddressOf(), &m_stride, &offset);
		}

	protected:
		COMPTR<ID3D11Buffer> m_pVertexBuffer;
		UINT m_stride;
	};
}