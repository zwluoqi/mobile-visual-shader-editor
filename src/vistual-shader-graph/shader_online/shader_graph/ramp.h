#pragma once

#include <cassert>
#include <cstddef>
#include <vector>

#include "../shader_core/vector.h"

namespace csg {
	struct ColorRampPoint {
	public:
		ColorRampPoint(float pos, csc::Float3 color, float alpha) : pos{ pos }, color{ color }, alpha{ alpha } {}

		float pos;
		csc::Float3 color;
		float alpha;
	};

	class ColorRamp {
	public:
		ColorRamp();
		ColorRamp(const std::vector<ColorRampPoint>& points);

		csc::Float4 eval(float pos) const;

		size_t size() const { return points.size(); }

		ColorRampPoint get(size_t index) const {
			assert(index < points.size());
			return points[index];
		}
		std::vector<ColorRampPoint> get() const { return points; }

		void set(size_t index, ColorRampPoint new_point);
		void remove(size_t index);

		bool similar(const ColorRamp& other, float margin) const;

	private:
		void sort_points();

		std::vector<ColorRampPoint> points;
	};
}
