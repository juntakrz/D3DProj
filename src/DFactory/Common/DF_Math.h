#pragma once

#include "../../DF_Settings.h"

namespace GMath
{
	// precise enough definition of PI as a 64 bit float
	constexpr double PI64 = 3.1415926535897932384626433832795028841971693993751;

	template<typename T>
	inline T WrapAngle(const T theta)		// wrap angle to -180 .. 180 degrees (-PI .. PI)
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

	template<typename T>
	constexpr T GaussDistribution(const T& x, const T& sigma) noexcept		// Gaussian distribution formula for blur
	{
		const auto ss = sigma * sigma;
		return ((T)1.0 / sqrt((T)2.0 * (T)PI64 * ss)) * exp(-(x * x) / ((T)2.0 * ss));
	}

	template<typename T>
	constexpr std::vector<T> GaussBlurKernel(uint8_t radius, T sigma) noexcept
	{
		// limit kernel radius to 3 .. 8 (gauss blur shader limitation)
		(radius > 8) ? radius = 8 : (radius < 3) ? radius = 3 : radius;

		// initialize data
		std::vector<T> gaussCoef;
		T sum = T(0.0);
		T core = T(0.0);

		// padding lambda
		auto emplace = [&](const T& var) {
			gaussCoef.emplace_back(var);
			for (uint8_t p = 0; p < 3; p++)
			{
				gaussCoef.emplace_back();
			}
		};

		// insert radius as the first variable
		emplace(T(radius));

		for (int8_t i = radius; i > -1; i--)
		{
			const T x = T(i - radius);
			const T g = GMath::GaussDistribution(x, sigma);
			(i != radius) ? sum += g : core = g;

			// emplace coefficient
			emplace(g);
		}

		// calculate final sum (everything but core repeats twice)
		sum = sum * T(2.0) + core;

		// normalize coefficients
		uint8_t steppedRadius = radius * 4;
		for (uint8_t j = 4; j < steppedRadius + 1; j += 4)
		{
			gaussCoef[j] /= sum;
		}

		return gaussCoef;
	}
}