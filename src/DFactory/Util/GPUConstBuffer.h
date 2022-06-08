#pragma once

namespace DF
{
	ID3D11Device* Device() noexcept;
	ID3D11DeviceContext* Context() noexcept;
}

class CBuffer
{
	std::vector<float> m_floatBuffer;
	bool m_firstCall = true;
	COMPTR<ID3D11Buffer> m_pBuffer;

public:
	// generate cbuffer using a vector
	template<typename T>
	void GenerateBuffer(std::vector<T>&& in_data)
	{
		// check if number of elements add up to multiples of 4 floats (16 bytes) and if not - add required padding
		int mod = 4 - (in_data.size() % 4);
		int paddedSize = sizeof(float) * (in_data.size() + mod);

		D3D11_BUFFER_DESC bd = {};
		bd.Usage = D3D11_USAGE_DYNAMIC;
		bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		bd.MiscFlags = 0u;
		bd.ByteWidth = paddedSize;
		bd.StructureByteStride = 0u;

		D3D11_SUBRESOURCE_DATA sd = {};
		sd.pSysMem = in_data.data();

		DF::Device()->CreateBuffer(&bd, &sd, &m_pBuffer);

		m_floatBuffer = std::move(in_data);
	}

	// when a single variable is left - uses this method to finish it up and create buffer
	template<typename T>
	void GenerateBuffer(T&& var) noexcept
	{
		m_floatBuffer.emplace_back(std::move(var));

		// check if number of elements add up to multiples of 4 floats (16 bytes) and if not - add required padding
		int mod = 4 - (m_floatBuffer.size() % 4);
		int paddedSize = sizeof(float) * (m_floatBuffer.size() + mod);
		for (int i = 0; i < mod; i++)
		{
			m_floatBuffer.emplace_back(0.0f);
		}

		D3D11_BUFFER_DESC bd = {};
		bd.Usage = D3D11_USAGE_DYNAMIC;
		bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		bd.MiscFlags = 0u;
		bd.ByteWidth = paddedSize;
		bd.StructureByteStride = 0u;

		D3D11_SUBRESOURCE_DATA sd = {};
		sd.pSysMem = m_floatBuffer.data();

		DF::Device()->CreateBuffer(&bd, &sd, &m_pBuffer);
	}

	// generates shader cbuffer with provided float stream, padding must be done manually
	template<typename T, typename ...Vars>
	void GenerateBuffer(T&& var, Vars&& ...vars)
	{
		if (m_firstCall)
		{
			m_floatBuffer.clear();	// clear the vector before sending in data
			m_firstCall = false;
		}
		m_floatBuffer.emplace_back(std::move(var));
		GenerateBuffer(std::forward<Vars>(vars)...);
	}

	// bind cbuffer to vertex shader at specified register
	void BindToVS(const uint8_t& nRegister) noexcept
	{
		DF::Context()->VSSetConstantBuffers(nRegister, 1u, m_pBuffer.GetAddressOf());
	}

	// bind cbuffer to geometry shader at specified register
	void BindToGS(const uint8_t& nRegister) noexcept
	{
		DF::Context()->GSSetConstantBuffers(nRegister, 1u, m_pBuffer.GetAddressOf());
	}

	// bind cbuffer to pixel shader at specified register
	void BindToPS(const uint8_t& nRegister) noexcept
	{
		DF::Context()->PSSetConstantBuffers(nRegister, 1u, m_pBuffer.GetAddressOf());
	}
};