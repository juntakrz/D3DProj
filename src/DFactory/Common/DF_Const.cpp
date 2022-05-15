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
}