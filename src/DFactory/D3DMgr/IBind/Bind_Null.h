#pragma once

#include "IBind.h"

// null buffers that effectively unbind buffers when Bind() is called

namespace Bind
{
	class Null_VertexShader : public IBind
	{
	public:
		Null_VertexShader() noexcept {};

		void Bind() noexcept override
		{
			GetContext()->VSSetShader(nullptr, nullptr, NULL);
		}

	};

	class Null_PixelShader : public IBind
	{
	public:
		Null_PixelShader() noexcept {};

		void Bind() noexcept override
		{
			GetContext()->PSSetShader(nullptr, nullptr, NULL);
		}
	};

	class Null_GeometryShader : public IBind
	{
	public:
		Null_GeometryShader() noexcept {};

		void Bind() noexcept override
		{
			GetContext()->GSSetShader(nullptr, nullptr, NULL);
		}
	};

	class Null_Texture : public IBind
	{
	public:
		Null_Texture() noexcept {};
		Null_Texture(UINT slot) noexcept : m_slot(slot) {};
		Null_Texture(UINT slot, UINT numViews) noexcept : m_slot(slot), m_numViews(numViews) {};

		void Bind() noexcept override
		{
			GetContext()->PSSetShaderResources(m_slot, m_numViews, m_pNullSRV);
		}

		static void Bind(const uint8_t& id, const UINT& slot, const UINT& numViews = 1) noexcept
		{
			ID3D11ShaderResourceView* pNullSRV = nullptr;
			switch (id)
			{
			case DF::ShaderType::GS:
			{
				GetContext()->GSSetShaderResources(slot, numViews, &pNullSRV);
				return;
			}
			case DF::ShaderType::PS:
			{
				GetContext()->PSSetShaderResources(slot, numViews, &pNullSRV);
				return;
			}
			}
		}

	private:
		UINT m_slot = 0u;
		UINT m_numViews = 1u;
		ID3D11ShaderResourceView* m_pNullSRV[4] = { nullptr, nullptr, nullptr, nullptr };
	};
}