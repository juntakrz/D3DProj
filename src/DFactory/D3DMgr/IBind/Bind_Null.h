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

	class Null_Texture : public IBind
	{
	public:
		Null_Texture() noexcept {};
		Null_Texture(UINT slot) noexcept : m_slot(slot) {};
		Null_Texture(UINT slot, UINT numViews) noexcept : m_slot(slot), m_numViews(numViews) {};

		void Bind() noexcept override
		{
			GetContext()->PSSetShaderResources(m_slot, m_numViews, nullptr);
		}

	private:
		UINT m_slot = 0u;
		UINT m_numViews = 1u;
	};
}