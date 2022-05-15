#pragma once

#include "IBind.h"

namespace Bind
{
	class Sampler : public IBind
	{
	public:
		enum class Mode
		{
			None		= 0,
			Wrap		= 1,
			Mirror		= 2,
			Clamp		= 3,
			Border		= 4,
			MirrorOnce	= 5
		};

		Sampler(Mode mode = Mode::Wrap);
		void Bind() noexcept override;

	protected:
		COMPTR<ID3D11SamplerState> m_pSampler;
	};
}