#pragma once

#include "IBind.h"

namespace Bind
{
	class Sampler : public IBind
	{
	public:
		enum Mode
		{
			None		= 0,
			Wrap		= 1,
			Mirror		= 2,
			Clamp		= 3,
			Border		= 4,
			MirrorOnce	= 5,
			Comparison  = 10	// used by hardware PCF sampler comparison state
		};

		Sampler(UINT mode = Mode::Wrap, UINT slot = 0u, UINT target = DF::ShaderType::PS);
		void Bind() noexcept override;

	protected:
		COMPTR<ID3D11SamplerState> m_pSampler;
		UINT m_slot = 0u;
		UINT m_target = DF::ShaderType::PS;
	};
}