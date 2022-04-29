#pragma once

#include "../../DF_Settings.h"

namespace GMath
{
	template<typename T>
	inline T WrapAngle(const T theta)
	{
		constexpr T PI2 = (T)2.0 * (T)PI64;
		const T remainder = fmod(theta, PI2);
		return (remainder > (T)PI64) ? remainder - PI2 : remainder;
	}

	template<typename T>
	inline T NormalizeMouseCoord(const T coord, const T dimension)
	{
		return (dimension > 0) ? coord / dimension * (T)2.0 - (T)1.0 : (coord / dimension * (T)2.0) + (T)1.0;
	}
}