#include "slot.h"

#include <cassert>
#include <cmath>

#include <boost/algorithm/clamp.hpp>
#include <boost/optional.hpp>

#include "../shader_core/config.h"
#include "../shader_core/rect.h"
#include "../shader_core/vector.h"

static bool set_curve(csg::Curve& curve, const csg::Curve& new_value)
{
	if (new_value.min() != curve.min() || new_value.max() != curve.max()) {
		return false;
	}
	curve = new_value;
	return true;
}

bool csg::ColorSlotValue::operator==(const ColorSlotValue& other) const
{
	return value.similar(other.value, FLOAT_COMPARE_DIFF);
}

void csg::EnumSlotValue::set(const size_t new_value)
{
	if (new_value <= max_value) {
		value = new_value;
	}
}

const char* csg::EnumSlotValue::internal_name() const
{
	boost::optional<NodeEnumOptionInfo> opt_info{ NodeEnumOptionInfo::from(meta_enum, value) };
	if (opt_info) {
		return opt_info->internal_name();
	}
	return "NAME ERROR";
}

bool csg::EnumSlotValue::operator==(const EnumSlotValue& other) const
{
	return (value == other.value && meta_enum == other.meta_enum);
}

void csg::FloatSlotValue::set(const float new_value)
{
	value = boost::algorithm::clamp(new_value, min, max);
}

bool csg::FloatSlotValue::operator==(const FloatSlotValue& other) const
{
	return (fabs(value - other.value) < FLOAT_COMPARE_DIFF) && min == other.min && max == other.max && _precision == other._precision;
}

void csg::IntSlotValue::set(const int new_value)
{
	value = boost::algorithm::clamp(new_value, min, max);
}

bool csg::IntSlotValue::operator==(const IntSlotValue& other) const
{
	return value == other.value && min == other.min && max == other.max;
}

void csg::VectorSlotValue::set(const csc::Float3 new_value)
{
	const float x{ boost::algorithm::clamp(new_value.x, min.x, max.x) };
	const float y{ boost::algorithm::clamp(new_value.y, min.y, max.y) };
	const float z{ boost::algorithm::clamp(new_value.z, min.z, max.z) };
	value = csc::Float3{ x, y, z };
}

bool csg::VectorSlotValue::operator==(const VectorSlotValue& other) const
{
	return value.similar(other.value, FLOAT_COMPARE_DIFF);
}

csg::RGBCurveSlotValue::RGBCurveSlotValue() :
	curve_all{ csc::Float2{ 0.0f, 0.0f }, csc::Float2{ 1.0f, 1.0f } },
	curve_r{ csc::Float2{ 0.0f, 0.0f }, csc::Float2{ 1.0f, 1.0f } },
	curve_g{ csc::Float2{ 0.0f, 0.0f }, csc::Float2{ 1.0f, 1.0f } },
	curve_b{ csc::Float2{ 0.0f, 0.0f }, csc::Float2{ 1.0f, 1.0f } }
{

}

bool csg::RGBCurveSlotValue::set_all(const Curve& value)
{
	return set_curve(curve_all, value);
}

bool csg::RGBCurveSlotValue::set_r(const Curve& value)
{
	return set_curve(curve_r, value);
}

bool csg::RGBCurveSlotValue::set_g(const Curve& value)
{
	return set_curve(curve_g, value);
}

bool csg::RGBCurveSlotValue::set_b(const Curve& value)
{
	return set_curve(curve_b, value);
}

bool csg::RGBCurveSlotValue::operator==(const RGBCurveSlotValue& other) const
{
	return (
		curve_all.similar(other.curve_all , FLOAT_COMPARE_DIFF) &&
		curve_r.similar(other.curve_r, FLOAT_COMPARE_DIFF) &&
		curve_g.similar(other.curve_g, FLOAT_COMPARE_DIFF) &&
		curve_b.similar(other.curve_b, FLOAT_COMPARE_DIFF)
		);
}

csg::VectorCurveSlotValue::VectorCurveSlotValue(const csc::Float2 min, const csc::Float2 max) :
	curve_x{ min, max },
	curve_y{ min, max },
	curve_z{ min, max },
	min{ min },
	max{ max }
{

}

bool csg::VectorCurveSlotValue::set_x(const Curve& value)
{
	return set_curve(curve_x, value);
}

bool csg::VectorCurveSlotValue::set_y(const Curve& value)
{
	return set_curve(curve_y, value);
}

bool csg::VectorCurveSlotValue::set_z(const Curve& value)
{
	return set_curve(curve_z, value);
}

bool csg::VectorCurveSlotValue::set_bounds(const csc::FloatRect bounds_rect)
{
	const bool valid_x{ curve_x.bounds_valid(bounds_rect) };
	const bool valid_y{ curve_y.bounds_valid(bounds_rect) };
	const bool valid_z{ curve_z.bounds_valid(bounds_rect) };
	if (valid_x && valid_y && valid_z) {
		curve_x.set_bounds(bounds_rect);
		curve_y.set_bounds(bounds_rect);
		curve_z.set_bounds(bounds_rect);
		min = bounds_rect.begin();
		max = bounds_rect.end();
		return true;
	}
	else {
		return false;
	}
}

bool csg::VectorCurveSlotValue::operator==(const VectorCurveSlotValue& other) const
{
	return (
		curve_x.similar(other.curve_x, FLOAT_COMPARE_DIFF) &&
		curve_y.similar(other.curve_y, FLOAT_COMPARE_DIFF) &&
		curve_z.similar(other.curve_z, FLOAT_COMPARE_DIFF) &&
		min.similar(other.min, FLOAT_COMPARE_DIFF) &&
		max.similar(other.max, FLOAT_COMPARE_DIFF)
	);
}

bool csg::ColorRampSlotValue::operator==(const ColorRampSlotValue& other) const
{
	return ramp.similar(other.ramp, FLOAT_COMPARE_DIFF);
}

bool csg::ImageSlotValue::operator==(const ImageSlotValue& other) const
{
    return strcmp(filePath, other.filePath) ==0;
}

csg::SlotValue& csg::SlotValue::operator=(const SlotValue& other)
{
	_type = other._type;
	value_union = other.value_union;

	if (other.curve_rgb_value) {
		curve_rgb_value = std::make_unique<RGBCurveSlotValue>(*other.curve_rgb_value);
	}
	else {
		curve_vector_value = std::unique_ptr<VectorCurveSlotValue>();
	}

	if (other.curve_vector_value) {
		curve_vector_value = std::make_unique<VectorCurveSlotValue>(*other.curve_vector_value);
	}
	else {
		curve_vector_value = std::unique_ptr<VectorCurveSlotValue>();
	}

	if (other.color_ramp_value) {
		color_ramp_value = std::make_unique<ColorRampSlotValue>(*other.color_ramp_value);
	}
	else {
		color_ramp_value = std::unique_ptr<ColorRampSlotValue>();
	}
    
    if (other.image_path_value) {
        image_path_value = std::make_unique<ImageSlotValue>(*other.image_path_value);
    }
    else {
        image_path_value = std::unique_ptr<ImageSlotValue>();
    }

	return *this;
}

template <> boost::optional<csg::BoolSlotValue> csg::SlotValue::as() const {
	return (type() != SlotType::BOOL) ? boost::none : boost::optional<csg::BoolSlotValue>{ value_union.bool_value };
}
template <> boost::optional<csg::ColorSlotValue> csg::SlotValue::as() const {
	return (type() != SlotType::COLOR) ? boost::none : boost::optional<csg::ColorSlotValue>{ value_union.color_value };
}
template <> boost::optional<csg::EnumSlotValue> csg::SlotValue::as() const {
	return (type() != SlotType::ENUM) ? boost::none : boost::optional<csg::EnumSlotValue>{ value_union.enum_value };
}
template <> boost::optional<csg::FloatSlotValue> csg::SlotValue::as() const {
	return (type() != SlotType::FLOAT) ? boost::none : boost::optional<csg::FloatSlotValue>{ value_union.float_value };
}
template <> boost::optional<csg::IntSlotValue> csg::SlotValue::as() const {
	return (type() != SlotType::INT) ? boost::none : boost::optional<csg::IntSlotValue>{ value_union.int_value };
}
template <> boost::optional<csg::VectorSlotValue> csg::SlotValue::as() const {
	return (type() != SlotType::VECTOR) ? boost::none : boost::optional<csg::VectorSlotValue>{ value_union.vector_value };
}
template <> boost::optional<csg::RGBCurveSlotValue> csg::SlotValue::as() const {
	return (type() != SlotType::CURVE_RGB || static_cast<bool>(curve_rgb_value) == false) ? boost::none : boost::optional<csg::RGBCurveSlotValue>{ *curve_rgb_value };
}
template <> boost::optional<csg::VectorCurveSlotValue> csg::SlotValue::as() const {
	return (type() != SlotType::CURVE_VECTOR || static_cast<bool>(curve_vector_value) == false) ? boost::none : boost::optional<csg::VectorCurveSlotValue>{ *curve_vector_value };
}
template <> boost::optional<csg::ColorRampSlotValue> csg::SlotValue::as() const {
	return (type() != SlotType::COLOR_RAMP || static_cast<bool>(color_ramp_value) == false) ? boost::none : boost::optional<csg::ColorRampSlotValue>{ *color_ramp_value };
}

template <> boost::optional<csg::ImageSlotValue> csg::SlotValue::as() const {
    return (type() != SlotType::IMAGE || static_cast<bool>(image_path_value) == false) ? boost::none : boost::optional<csg::ImageSlotValue>{ *image_path_value };
}

bool csg::SlotValue::operator==(const SlotValue& other) const
{
	if (_type != other._type) {
		return false;
	}

	switch (_type) {
		case SlotType::BOOL:
		{
			if (value_union.bool_value != other.value_union.bool_value) {
				return false;
			}
			break;
		}
		case SlotType::COLOR:
		{
			if (value_union.color_value != other.value_union.color_value) {
				return false;
			}
			break;
		}
		case SlotType::ENUM:
		{
			if (value_union.enum_value != other.value_union.enum_value) {
				return false;
			}
			break;
		}
		case SlotType::FLOAT:
		{
			if (value_union.float_value != other.value_union.float_value) {
				return false;
			}
			break;
		}
		case SlotType::INT:
		{
			if (value_union.int_value != other.value_union.int_value) {
				return false;
			}
			break;
		}
		case SlotType::VECTOR:
		{
			if (value_union.vector_value != other.value_union.vector_value) {
				return false;
			}
			break;
		}
		case SlotType::CURVE_RGB:
		{
			assert(curve_rgb_value.get() != nullptr);
			assert(other.curve_rgb_value.get() != nullptr);
			if (*curve_rgb_value != *other.curve_rgb_value) {
				return false;
			}
			break;
		}
		case SlotType::CURVE_VECTOR:
		{
			assert(curve_vector_value.get() != nullptr);
			assert(other.curve_vector_value.get() != nullptr);
			if (*curve_vector_value != *other.curve_vector_value) {
				return false;
			}
			break;
		}
		case SlotType::COLOR_RAMP:
		{
			assert(color_ramp_value.get() != nullptr);
			assert(other.color_ramp_value.get() != nullptr);
			if (*color_ramp_value != *other.color_ramp_value) {
				return false;
			}
			break;
		}
        case SlotType::IMAGE:
        {
            assert(image_path_value.get() != nullptr);
            assert(other.image_path_value.get() != nullptr);
            if (*image_path_value != *other.image_path_value) {
                return false;
            }
            break;
        }
		default:
			// This should only happen when a new unsupported type is added
			assert(false);
	}

	return true;
}

bool csg::Slot::operator==(const Slot& other) const
{
	if (_name != other._name) {
		return false;
	}

	if (_dir != other._dir) {
		return false;
	}

	if (_type != other._type) {
		return false;
	}

	if (value.has_value() != other.value.has_value()) {
		return false;
	}

	if (value.has_value()) {
		if (*value != *(other.value)) {
			return false;
		}
	}

	return true;
}
