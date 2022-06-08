#include "../../pch.h"
#include "Util.h"

namespace Util
{
	const std::wstring& ToWString(std::string string)
	{
		return std::wstring(string.begin(), string.end());
	}

	uint64_t FlagPow(const uint64_t& power)
	{
		uint64_t result = 1;

		for (uint64_t i = 0; i < power; i++)
		{
			result *= 2;
		}

		return result;
	}
}