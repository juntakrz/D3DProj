#include "../../pch.h"
#include "DF_Const.h"

const std::wstring DF::Default::map = L"default";
const std::string DF::Default::material = "Mat_Default";

namespace DF
{
	// variable definitions
	bool isCullingEnabled = true;
	uint64_t framesRendered = 0;
	bool isLoadScreen = false;

	namespace CSM
	{
		float bufferSize = 2048.0f;
		float FOVmult = 5.5f;
		float minZ = 0.01f;
		float maxZ = 48.0f;
	}

	// function definitions
	DXGI_FORMAT GetDepthFormatTyped(const uint8_t& usage) noexcept
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

	DXGI_FORMAT GetDepthFormatTypeless(const uint8_t& usage) noexcept
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

	DXGI_FORMAT GetDepthFormatColor(const uint8_t& usage) noexcept
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

	std::string Pass::IdToString(const uint32_t& id) noexcept
	{
		switch (id)
		{
		case None:			return "None";
		case Shadow:		return "Shadow";
		case Occlusion:		return "Occlusion";
		case Background:	return "Background";
		case Standard:		return "Standard";
		case Blur:			return "Blur";
		case PointSprites:	return "PointSprites";
		case AABBShow:		return "AABBShow";
		case Masking:		return "Masking";
		case Outline:		return "Outline";
		}
	}
}