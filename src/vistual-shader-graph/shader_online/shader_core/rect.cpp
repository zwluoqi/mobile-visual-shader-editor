#include "rect.h"

#include <algorithm>

#include <boost/algorithm/clamp.hpp>

csc::FloatRect::FloatRect(const Float2 p1, const Float2 p2)
{
	const float min_x{ std::min(p1.x, p2.x) };
	const float max_x{ std::max(p1.x, p2.x) };

	const float min_y{ std::min(p1.y, p2.y) };
	const float max_y{ std::max(p1.y, p2.y) };

	const float size_x{ max_x - min_x };
	const float size_y{ max_y - min_y };

	_begin = Float2{ min_x, min_y };
	_size = Float2{ size_x, size_y };
}

csc::Float2 csc::FloatRect::clamp(const csc::Float2 point) const
{
	const float result_x{ boost::algorithm::clamp(point.x, begin().x, end().x) };
	const float result_y{ boost::algorithm::clamp(point.y, begin().y, end().y) };
	return csc::Float2{ result_x, result_y };
}

csc::Float2 csc::FloatRect::normalized_pos(const csc::Float2 point) const
{
	const csc::Float2 relative_pos{ point - begin() };
	const csc::Float2 result{ relative_pos.x / size().x, relative_pos.y / size().y };
	return result;
}

csc::Float2 csc::FloatRect::scaled_pos(const csc::Float2 point) const
{
	const float x{ point.x * (end().x - begin().x) + begin().x };
	const float y{ point.y * (end().y - begin().y) + begin().y };
	return Float2{ x, y };
}

csc::FloatRect csc::FloatRect::grow(const float amount) const
{
	const Float2 delta{ amount, amount };
	const Float2 new_begin{ begin() - delta };
	const Float2 new_end{ end() + delta };
	return FloatRect{ new_begin, new_end };
}

csc::FloatRect csc::FloatRect::with_point(const Float2 point) const
{
	if (contains(point)) {
		// This rectangle already contains the point
		return FloatRect{ *this };
	}

	// Return the bounding box of p0, p1, and p2
	const Float2 p0{ begin() };
	const Float2 p1{ end() };
	const Float2 p2{ point };

	const float min_x{ std::min(std::min(p0.x, p1.x), p2.x) };
	const float min_y{ std::min(std::min(p0.y, p1.y), p2.y) };

	const float max_x{ std::max(std::max(p0.x, p1.x), p2.x) };
	const float max_y{ std::max(std::max(p0.y, p1.y), p2.y) };

	const Float2 begin{ min_x, min_y };
	const Float2 end{ max_x, max_y };

	return FloatRect{ begin, end };
}

bool csc::FloatRect::contains(const Float2 point) const
{
	const bool x{ point.x >= _begin.x && point.x <= end().x };
	const bool y{ point.y >= _begin.y && point.y <= end().y };
	return x && y;
}

bool csc::FloatRect::overlaps(const FloatRect other) const
{
	const auto this_end{ end() };
	const auto other_end{ other.end() };

	// Check horizontal overlap
	if (_begin.x > other_end.x || other._begin.x > this_end.x) {
		return false;
	}

	// Check vertical overlap
	if (_begin.y > other_end.y || other._begin.y > this_end.y) {
		return false;
	}

	return true;
}
