#pragma once

#include "IBind.h"

namespace Bind
{

	//constant buffer is a structure custom defined, so needs to be templated
	template <typename C>
	class ConstBuffer : public IBind
	{
	public:
		ConstBuffer(UINT slot = 0)
			: m_slot(slot)
		{
			D3D_DXGIDEBUG(*DF::DFM);

			D3D11_BUFFER_DESC bd = {};
			bd.Usage = D3D11_USAGE_DYNAMIC;
			bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
			bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
			bd.MiscFlags = 0u;
			bd.ByteWidth = sizeof(C);
			bd.StructureByteStride = 0u;				//not an array

			D3D_THROW(GetDevice()->CreateBuffer(&bd, nullptr, &m_pConstBuffer));
		};
		ConstBuffer(const C& cbuf, UINT slot = 0)
			: m_slot(slot)
		{
			D3D_DXGIDEBUG(*DF::DFM);

			D3D11_BUFFER_DESC bd = {};
			bd.Usage = D3D11_USAGE_DYNAMIC;
			bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
			bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
			bd.MiscFlags = 0u;
			bd.ByteWidth = sizeof(cbuf);
			bd.StructureByteStride = 0u;				//not an array

			D3D11_SUBRESOURCE_DATA sd = {};
			sd.pSysMem = &cbuf;

			D3D_THROW(GetDevice()->CreateBuffer(&bd, &sd, &m_pConstBuffer));
		};
		void Update(const C& cbuf)
		{
			D3D_DXGIDEBUG(*DF::DFM);

			D3D11_MAPPED_SUBRESOURCE msr = {};
			D3D_THROW(GetContext()->Map(m_pConstBuffer.Get(), 0u, D3D11_MAP_WRITE_DISCARD, 0u, &msr));
			memcpy(msr.pData, &cbuf, sizeof(cbuf));
			GetContext()->Unmap(m_pConstBuffer.Get(), 0u);
		};
		
		void Update(const std::vector<C>* cbuf, int size)
		{
			D3D_DXGIDEBUG(*DF::DFM);

			std::vector<C> tbuf(cbuf->size());
			memcpy(tbuf.data(), cbuf->data(), sizeof(C) * cbuf->size());

			D3D11_MAPPED_SUBRESOURCE msr = {};
			D3D_THROW(GetContext()->Map(m_pConstBuffer.Get(), 0u, D3D11_MAP_WRITE_DISCARD, 0u, &msr));
			memcpy(msr.pData, tbuf.data(), sizeof(C) * cbuf->size());
			GetContext()->Unmap(m_pConstBuffer.Get(), 0u);
		};

	protected:
		COMPTR<ID3D11Buffer> m_pConstBuffer;
		UINT m_slot;
	};

	template <typename C>
	class ConstVertexBuffer : public ConstBuffer<C>
	{
		using ConstBuffer<C>::m_pConstBuffer;
		using ConstBuffer<C>::m_slot;
		using IBind::GetContext;
	public:
		using ConstBuffer<C>::ConstBuffer;
		void Bind() noexcept override
		{
			GetContext()->VSSetConstantBuffers(m_slot, 1u, m_pConstBuffer.GetAddressOf());
		};
	};

	template <typename C>
	class ConstPixelBuffer : public ConstBuffer<C>
	{
		//template class shennanigans
		using ConstBuffer<C>::m_pConstBuffer;
		using ConstBuffer<C>::m_slot;
		using IBind::GetContext;
	public:
		using ConstBuffer<C>::ConstBuffer;
		void Bind() noexcept override
		{
			GetContext()->PSSetConstantBuffers(m_slot, 1u, m_pConstBuffer.GetAddressOf());
		};
	};
}