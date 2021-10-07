#pragma once

/**
 * @file
 * @brief Defines FloatRect.
 */

#include "vector.h"

namespace csc {

	/**
	 * @brief Used to represent some rectangle in 2d space.
	 */
	class FloatRect {
	public:
		FloatRect(Float2 p1, Float2 p2);

		inline Float2 begin() const { return _begin; }
		inline Float2 end() const { return _begin + _size; }
		inline Float2 size() const { return _size; }

		inline float area() const { return _size.x * _size.y; }
		inline Float2 center() const { return _begin + _size / 2.0f; }

		Float2 clamp(Float2 point) const;
		// Map from [min, max] to [0, 1]
		Float2 normalized_pos(Float2 point) const;
		// Map from [0, 1] to [min, max]
		Float2 scaled_pos(Float2 point) const;

		FloatRect grow(float amount) const;
		FloatRect with_point(Float2 point) const;

		bool contains(Float2 point) const;
		bool overlaps(FloatRect other) const;

	private:
		Float2 _begin;
		Float2 _size;
	};
}
