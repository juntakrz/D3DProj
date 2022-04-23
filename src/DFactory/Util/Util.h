#pragma once

#include "../../pch.h"

namespace Util
{
	const std::wstring& ToWString(std::string string)
	{
		return std::wstring(string.begin(), string.end());
	}
}