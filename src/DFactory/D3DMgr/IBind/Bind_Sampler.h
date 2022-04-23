#pragma once

#include "IBind.h"

namespace Bind
{
	class Sampler : public IBind
	{
	public:
		Sampler(D3DMgr& d3dMgr);
		void Bind(D3DMgr& d3dMgr) noexcept override;

	protected:
		COMPTR<ID3D11SamplerState> m_pSampler;
	};
}