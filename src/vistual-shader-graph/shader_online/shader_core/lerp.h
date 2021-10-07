#pragma once

namespace csc {

	template <typename T> T lerp(const T& a, const T& b, const float t)
	{
		return a + (b - a) * t;
	}
}
