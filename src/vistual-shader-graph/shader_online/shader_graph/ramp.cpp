#include "ramp.h"

#include <algorithm>
#include <cassert>
#include <cmath>
#include <cstddef>

#include <boost/algorithm/clamp.hpp>

#include "../shader_core/lerp.h"

csg::ColorRamp::ColorRamp()
{
	// Set a default value mapping from black to white
	points.push_back(ColorRampPoint{ 0.0f, csc::Float3{ 0.0f, 0.0f, 0.0f }, 1.0f });
	points.push_back(ColorRampPoint{ 1.0f, csc::Float3{ 1.0f, 1.0f, 1.0f }, 1.0f });
	sort_points();
}

csg::ColorRamp::ColorRamp(const std::vector<ColorRampPoint>& points) : points{ points }
{
	assert(points.size() >= 2);
	sort_points();
}

csc::Float4 csg::ColorRamp::eval(const float pos) const
{
	// For speed, assume the vector is already sorted
	assert(points.size() >= 2);

	// If the pos is outside our control points, extend the boundary color
	const float pos_begin{ points[0].pos };
	if (pos <= pos_begin) {
		return csc::Float4{ points[0].color, points[0].alpha };
	}
	const float pos_end{ points[points.size() - 1].pos };
	if (pos >= pos_end) {
		return csc::Float4{ points[points.size() - 1].color, points[points.size() - 1].alpha };
	}

	// Find the bounding points
	// We know that a pos both before and after the input position must exist
	csg::ColorRampPoint point_before{ points[0] };
	csg::ColorRampPoint point_after{ points[points.size() - 1] };
	for (const csg::ColorRampPoint& this_point : points) {
		if (this_point.pos > point_before.pos && this_point.pos < pos) {
			point_before = this_point;
		}
		if (this_point.pos < point_after.pos && this_point.pos > pos) {
			point_after = this_point;
		}
	}

	const float fraction{ (pos - point_before.pos) / (point_after.pos - point_before.pos) };

	const csc::Float4 result_before{ point_before.color, point_before.alpha };
	const csc::Float4 result_after{ point_after.color, point_after.alpha };

	return csc::lerp(result_before, result_after, fraction);
}

void csg::ColorRamp::set(const size_t index, ColorRampPoint new_point)
{
	if (index < points.size()) {
		new_point.pos = boost::algorithm::clamp(new_point.pos, 0.0f, 1.0f);
		points[index] = new_point;
		sort_points();
	}
}

void csg::ColorRamp::remove(size_t index)
{
	if (points.size() < 3) {
		// There must be 2 points remaining after deletion
		return;
	}

	if (index >= points.size()) {
		// Illegal index
		return;
	}

	points.erase(points.begin() + index);
	sort_points();
}

bool csg::ColorRamp::similar(const ColorRamp& other, const float margin) const
{
	if (points.size() != other.points.size()) {
		return false;
	}

	for (size_t i = 0; i < points.size(); i++) {
		const ColorRampPoint p1{ points[i] };
		const ColorRampPoint p2{ other.points[i] };
		const float diff_pos{ static_cast<float>(fabs(p1.pos - p2.pos)) };
		const float diff_alpha{ static_cast<float>(fabs(p1.alpha - p2.alpha)) };
		if (diff_pos > margin) {
			return false;
		}
		if (diff_alpha > margin) {
			return false;
		}
		if (p1.color.similar(p2.color, margin) == false) {
			return false;
		}
	}

	return true;
}

void csg::ColorRamp::sort_points()
{
	const auto lt_pos = [](const ColorRampPoint a, const ColorRampPoint b) -> bool {
		if (a.pos < b.pos) return true;
		else if (b.pos < a.pos) return false;

		if (a.color < b.color) return true;
		else if (b.color < a.color) return false;

		if (a.alpha < b.alpha) return true;
		else if (b.alpha < a.alpha) return false;

		return false;
	};
	std::sort(points.begin(), points.end(), lt_pos);
}
