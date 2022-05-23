#include "../../pch.h"
#include "DF_Const.h"

namespace DF
{
	DXGI_FORMAT GetDepthFormatTyped(const DS_Usage& usage) noexcept
	{

		switch (usage)
		{
		case DS_Usage::DepthStencil:
		{
			return DXGI_FORMAT::DXGI_FORMAT_D24_UNORM_S8_UINT;
		}
		case DS_Usage::DepthShadow:
		{
			return DXGI_FORMAT::DXGI_FORMAT_D32_FLOAT;
		}
		}

	};

	DXGI_FORMAT GetDepthFormatTypeless(const DS_Usage& usage) noexcept
	{
		switch (usage)
		{
		case DS_Usage::DepthStencil:
		{
			return DXGI_FORMAT::DXGI_FORMAT_R24G8_TYPELESS;
		}
		case DS_Usage::DepthShadow:
		{
			return DXGI_FORMAT::DXGI_FORMAT_R32_TYPELESS;
		}
		}
	}

	DXGI_FORMAT GetDepthFormatColor(const DS_Usage& usage) noexcept
	{
		switch (usage)
		{
		case DS_Usage::DepthStencil:
		{
			return DXGI_FORMAT::DXGI_FORMAT_R24_UNORM_X8_TYPELESS;
		}
		case DS_Usage::DepthShadow:
		{
			return DXGI_FORMAT::DXGI_FORMAT_R32_FLOAT;
		}
		}
	}

	namespace CSM
	{
		float bufferSize = 2048.0f;
		float FOVmult	 = 6.0f;
		float minZ		 = 0.01f;
		float maxZ		 = 48.0f;
	}

	bool isCullingEnabled = true;
	uint64_t framesRendered = 0;
}