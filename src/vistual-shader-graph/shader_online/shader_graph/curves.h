#pragma once

#include <array>
#include <cstddef>
#include <vector>

#include <boost/optional.hpp>

#include "../shader_core/vector.h"

namespace csc {
	class FloatRect;
}

namespace csg {
	enum class CurveInterp {
		CUBIC_HERMITE,
		LINEAR,
		COUNT,
	};

	struct CurvePoint {
		CurvePoint(csc::Float2 pos, CurveInterp interp) : pos{ pos }, interp{ interp } {}

		bool operator==(const CurvePoint& other) const;
		bool operator!=(const CurvePoint& other) const { return operator==(other) == false; }

		csc::Float2 pos;
		CurveInterp interp;
	};

	class Curve {
	public:
		Curve(csc::Float2 min, csc::Float2 max, boost::optional<std::vector<CurvePoint>> points = boost::none);

		const std::vector<CurvePoint>& control_points() const { return m_points; }
		size_t control_points_size() const { return m_points.size(); }
		csc::Float2 min() const { return _min; }
		csc::Float2 max() const { return _max; }

		float eval_point(float input) const;

		void delete_point(size_t index);
		size_t create_point(float x);
		size_t move_point(size_t index, csc::Float2 new_pos);

		void set_interp(size_t index, csg::CurveInterp new_interp);

		void set_bounds(csc::FloatRect bounds_rect);
		// Returns true if it is possible to apply the provided bounds to this curve
		bool bounds_valid(csc::FloatRect bounds_rect);

		bool similar(const Curve& other, float margin) const;

		bool operator==(const Curve& other) const;
		bool operator!=(const Curve& other) const { return operator==(other) == false; }

		template <size_t SIZE> std::array<float, SIZE> eval_curve() const
		{
			std::array<float, SIZE> result;
			for (size_t i = 0; i < SIZE; i++) {
				const float fraction{ static_cast<float>(i) / static_cast<float>(SIZE - 1) };
				const float x{ _min.x + (_max.x - _min.x) * fraction };
				result[i] = eval_point(x);
			}
			return result;
		}
	private:
		void sort_points();

		csc::Float2 _min;
		csc::Float2 _max;
		std::vector<CurvePoint> m_points;
	};
}
