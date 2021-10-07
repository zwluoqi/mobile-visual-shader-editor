#include "curves.h"

#include <algorithm>
#include <array>
#include <cassert>
#include <cstddef>
#include <cstdint>
#include <memory>

#include <boost/optional.hpp>

#include "../shader_core/lerp.h"
#include "../shader_core/rect.h"

static float eval_hermite_interpolation(
	const csc::Float2 p0,
	const csc::Float2 p1,
	const csc::Float2 p2,
	const csc::Float2 p3,
	const float input_x )
{
	std::array<csc::Float2, 32> hermite_curve;

	const auto do_hermite = [](const float p0, const float p1, const float p2, const float p3, const float t) -> float
	{
		const float a{ -p0 / 2.0f + (3.0f * p1) / 2.0f - (3.0f * p2) / 2.0f + p3 / 2.0f };
		const float b{ p0 - (5.0f * p1) / 2.0f + 2.0f * p2 - p3 / 2.0f };
		const float c{ -p0 / 2.0f + p2 / 2.0f };
		const float d{ p1 };

		return a * t*t*t + b * t*t + c * t + d;
	};

	// Assert that x values are in strictly ascending order
	assert(p1.x >= p0.x);
	assert(p2.x >= p1.x);
	assert(p3.x >= p2.x);

	// Populate our whole curve with (x, y) pairs
	for (size_t i = 0; i < hermite_curve.size(); i++) {
		const float t{ static_cast<float>(i) / (hermite_curve.size() - 1) };
		const float x{ do_hermite(p0.x, p1.x, p2.x, p3.x, t) };
		const float y{ do_hermite(p0.y, p1.y, p2.y, p3.y, t) };
		hermite_curve[i] = csc::Float2{ x, y };
	}

	// Look for the two (x, y) pairs that surround our x value
	// The curve is already known to be sorted on x
	size_t before_index{ SIZE_MAX };
	for (size_t i = 0; i < hermite_curve.size(); i++) {
		if (hermite_curve[i].x < input_x) {
			before_index = i;
		}
	}

	// In the case where our x input is before the first point, just return the value of the first point
	// This may happen due to floating point oddities
	if (before_index == SIZE_MAX) {
		return hermite_curve[0].y;
	}

	// In the case where our x input is beyond the last point, just return the value of the last point
	// This may happen due to floating point oddities
	if (before_index == hermite_curve.size() - 1) {
		return hermite_curve[hermite_curve.size() - 1].y;
	}

	const float lerp_t{ (input_x - hermite_curve[before_index].x) / (hermite_curve[before_index + 1].x - hermite_curve[before_index].x) };
	return csc::lerp(hermite_curve[before_index].y, hermite_curve[before_index + 1].y, lerp_t);
}

bool csg::CurvePoint::operator==(const CurvePoint& other) const
{
	return pos == other.pos && interp == other.interp;
}

csg::Curve::Curve(const csc::Float2 min, const csc::Float2 max, const boost::optional<std::vector<CurvePoint>> points) : _min{ min }, _max{ max }
{
	if (points) {
		// Validate that all points are inside the bounds
		const csc::FloatRect bounds{ min, max };
		for (const CurvePoint& this_point : *points) {
			assert(bounds.contains(this_point.pos));
		}
		this->m_points = *points;
	}
	else {
		// Default value is always a straight line from min to max
		this->m_points.push_back(csg::CurvePoint{ min, CurveInterp::CUBIC_HERMITE });
		this->m_points.push_back(csg::CurvePoint{ max, CurveInterp::CUBIC_HERMITE });
	}
	sort_points();
}

float csg::Curve::eval_point(const float input) const
{
	// For speed, this function assumes the control point vector is sorted already
	assert(m_points.size() >= 1);

	// Variable naming convention for points:
	// p1 is the point immediately before input
	// p2 is the point immediately after input
	// In the case of spline interpolation, we need 2 more
	// p0 is the point before p1
	// p3 is the point after p2
	// Any one of the points might not be real, but at least one of p1 or p2 must be real

	boost::optional<size_t> p1_index;
	boost::optional<size_t> p2_index;
	for (size_t i = 0; i < m_points.size(); i++) {
		// Find the point before input
		if (m_points[i].pos.x <= input) {
			p1_index = i;
		}
	}
	for (size_t i = m_points.size(); i > 0; i--) {
		// Find the point after input
		if (m_points[i-1].pos.x > input) {
			p2_index = i-1;
		}
	}

	// Clamp if input is not between two points
	if (p1_index.has_value() == false) {
		return m_points[0].pos.y;
	}
	if (p2_index.has_value() == false) {
		return m_points[m_points.size() - 1].pos.y;
	}

	const csc::Float2 p1{ m_points[*p1_index].pos };
	const csc::Float2 p2{ m_points[*p2_index].pos };
	const csg::CurveInterp p1_interp{ m_points[*p1_index].interp };
	const csg::CurveInterp p2_interp{ m_points[*p2_index].interp };

	const float delta_x{ p2.x - p1.x };
	const float fade{ (input - p1.x) / delta_x };

	const float linear_result{ csc::lerp(p1.y, p2.y, fade) };
	if (p1_interp == csg::CurveInterp::LINEAR && p2_interp == csg::CurveInterp::LINEAR) {
		// End early if possible to avoid doing unnecessary spline math
		return linear_result;
	}

	// Lambdas to generate p0 and p3 for our interp
	const auto gen_p0_pos = [](const std::vector<CurvePoint>& points, const size_t p1_index, const size_t p2_index) -> csc::Float2
	{
		if (p1_index == 0) {
			const csc::Float2 p1{ points[p1_index].pos };
			const csc::Float2 p2{ points[p2_index].pos };
			return p1 - (p2 - p1);
		}
		else {
			return points[p1_index - 1].pos;
		}
	};
	const auto gen_p3_pos = [](const std::vector<CurvePoint>& points, const size_t p1_index, const size_t p2_index) -> csc::Float2
	{
		if (p2_index >= points.size() - 1) {
			const csc::Float2 p1{ points[p1_index].pos };
			const csc::Float2 p2{ points[p2_index].pos };
			return p2 + (p2 - p1);
		}
		else {
			return points[p2_index + 1].pos;
		}
	};
	const csc::Float2 p0{ gen_p0_pos(m_points, *p1_index, *p2_index) };
	const csc::Float2 p3{ gen_p3_pos(m_points, *p1_index, *p2_index) };

	// Hermite spline interpolation

	const float hermite_result{ eval_hermite_interpolation(p0, p1, p2, p3, input) };

	// 0 means to use fully linear interp, 1 means fully hermite
	const auto calc_interp_lerp = [](const csg::CurveInterp p1_interp, const csg::CurveInterp p2_interp, const float t) -> float
	{
		if (p1_interp == csg::CurveInterp::LINEAR && p2_interp == csg::CurveInterp::LINEAR) {
			return 0.0f;
		}
		else if (p1_interp == csg::CurveInterp::CUBIC_HERMITE && p2_interp == csg::CurveInterp::CUBIC_HERMITE) {
			return 1.0f;
		}
		else if (p1_interp == csg::CurveInterp::CUBIC_HERMITE && p2_interp == csg::CurveInterp::LINEAR) {
			return 1.0f - t;
		}
		else if (p1_interp == csg::CurveInterp::LINEAR && p2_interp == csg::CurveInterp::CUBIC_HERMITE) {
			return t;
		}
		else {
			assert(false);
			// To suppress a compiler warning
			return 0.0f;
		}
	};

	const float interp_t{ (input - p1.x) / (p2.x - p1.x) };
	return csc::lerp(linear_result, hermite_result, calc_interp_lerp(p1_interp, p2_interp, interp_t));
}

void csg::Curve::delete_point(const size_t index)
{
	if (m_points.size() < 2) {
		// Curve is in a meaningless state
		assert(false);
	}
	else if (m_points.size() == 2) {
		return;
	}

	m_points.erase(m_points.begin() + index);
	sort_points();
}

size_t csg::Curve::create_point(const float x)
{
	if (x < _min.x) {
		return 0;
	}
	if (x > _max.x) {
		return m_points.size() - 1;
	}
	const float y{ eval_point(x) };
	const CurvePoint new_point{ csc::Float2{ x, y }, csg::CurveInterp::LINEAR };
	m_points.push_back(new_point);
	sort_points();
	// Use linear interp by default
	// If either surrounding point is hermite, then use hermite
	for (size_t i = 0; i < m_points.size(); i++) {
		if (m_points[i] == new_point) {
			// i is the index of the new point
			csg::CurveInterp interp{ csg::CurveInterp::LINEAR };
			if (i > 0 && m_points[i - 1].interp == csg::CurveInterp::CUBIC_HERMITE) {
				interp = csg::CurveInterp::CUBIC_HERMITE;
			}
			if (i + 1 < m_points.size() && m_points[i + 1].interp == csg::CurveInterp::CUBIC_HERMITE) {
				interp = csg::CurveInterp::CUBIC_HERMITE;
			}
			m_points[i].interp = interp;
			return i;
		}
	}
	assert(false);
	return 0;
}

size_t csg::Curve::move_point(const size_t index, const csc::Float2 new_pos)
{
	if (index >= m_points.size()) {
		return index;
	}

	const csc::FloatRect valid_pos{ _min, _max };
	m_points[index].pos = valid_pos.clamp(new_pos);
	const CurvePoint changed_point{ m_points[index] };
	sort_points();

	// Find and return the point's new index
	for (size_t i = 0; i < m_points.size(); i++) {
		if (changed_point == m_points[i]) {
			return i;
		}
	}
	assert(false);
	return index;
}

void csg::Curve::set_interp(const size_t index, const CurveInterp new_interp)
{
	if (index >= m_points.size()) {
		return;
	}
	m_points[index].interp = new_interp;
}

void csg::Curve::set_bounds(const csc::FloatRect bounds_rect)
{
	if (bounds_valid(bounds_rect) == false) {
		return;
	}
	_min = bounds_rect.begin();
	_max = bounds_rect.end();
}

bool csg::Curve::bounds_valid(const csc::FloatRect bounds_rect)
{
	for (const CurvePoint& this_point : m_points) {
		if (bounds_rect.contains(this_point.pos) == false) {
			return false;
		}
	}
	return true;
}

bool csg::Curve::similar(const Curve& other, const float margin) const
{
	if (_min.similar(other._min, margin) == false) {
		return false;
	}
	if (_max.similar(other._max, margin) == false) {
		return false;
	}
	if (m_points.size() != other.m_points.size()) {
		return false;
	}
	for (size_t i = 0; i < m_points.size(); i++) {
		if (m_points[i].interp != other.m_points[i].interp) {
			return false;
		}
		if (m_points[i].pos.similar(other.m_points[i].pos, margin) == false) {
			return false;
		}
	}

	return true;
}

bool csg::Curve::operator==(const Curve& other) const
{
	if (_min != other._min) {
		return false;
	}
	if (_max != other._max) {
		return false;
	}
	if (m_points.size() != other.m_points.size()) {
		return false;
	}
	for (size_t i = 0; i < m_points.size(); i++) {
		if (m_points[i] != other.m_points[i]) {
			return false;
		}
	}

	return true;
}

void csg::Curve::sort_points()
{
	const auto lt_x = [](const CurvePoint a, const CurvePoint b) -> bool {
		return a.pos.x < b.pos.x;
	};
	std::sort(m_points.begin(), m_points.end(), lt_x);
}
