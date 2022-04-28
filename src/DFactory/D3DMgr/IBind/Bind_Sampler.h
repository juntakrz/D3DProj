#pragma once

#include "IBind.h"

namespace Bind
{
	class Sampler : public IBind
	{
	public:
		Sampler();
		void Bind() noexcept override;

	protected:
		COMPTR<ID3D11SamplerState> m_pSampler;
	};
}