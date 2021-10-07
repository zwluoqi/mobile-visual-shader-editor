#include "alt_slot_names.h"

#include <algorithm>

boost::string_view cse::get_alt_slot_name(const csg::Node& node, const boost::string_view& disp_name)
{
	const char* const unused = "(unused)";
	switch (node.type()) {
	case csg::NodeType::MAP_RANGE:
		if (const auto opt_value{ node.slot_value_as<csg::EnumSlotValue>("type") }) {
			const csg::MapRangeType type{ static_cast<csg::MapRangeType>(opt_value->get()) };
			if (disp_name == "Steps") {
				if (type != csg::MapRangeType::STEPPED) {
					return unused;
				}
			}
			else if (disp_name == "Clamp") {
				if (type == csg::MapRangeType::SMOOTH_STEP || type == csg::MapRangeType::SMOOTHER_STEP) {
					return unused;
				}
			}
			else {
				return disp_name;
			}
		}
		break;
	case csg::NodeType::MATH:
		if (const auto opt_value{ node.slot_value_as<csg::EnumSlotValue>("type") }) {
			const size_t type_int = opt_value->get();
			const csg::MathType type{ static_cast<csg::MathType>(type_int) };
			switch (type) {
			case csg::MathType::MULTIPLY_ADD:
			{
				if (disp_name == "Value1") {
					return "Value";
				}
				else if (disp_name == "Value2") {
					return "Multiplier";
				}
				else if (disp_name == "Value3") {
					return "Addend";
				}
				break;
			}
			case csg::MathType::POWER:
			{
				if (disp_name == "Value1") {
					return "Base";
				}
				else if (disp_name == "Value2") {
					return "Exponent";
				}
				else if (disp_name == "Value3") {
					return unused;
				}
				break;
			}
			case csg::MathType::LOGARITHM:
			{
				if (disp_name == "Value1") {
					return "Value";
				}
				else if (disp_name == "Value2") {
					return "Base";
				}
				else if (disp_name == "Value3") {
					return unused;
				}
				break;
			}
			case csg::MathType::LESS_THAN:
			case csg::MathType::GREATER_THAN:
			{
				if (disp_name == "Value1") {
					return "Value";
				}
				else if (disp_name == "Value2") {
					return "Threshold";
				}
				else if (disp_name == "Value3") {
					return unused;
				}
				break;
			}
			case csg::MathType::DEGREES:
			{
				if (disp_name == "Value1") {
					return "Radians";
				}
				else if (disp_name == "Value2") {
					return unused;
				}
				else if (disp_name == "Value3") {
					return unused;
				}
				break;
			}
			case csg::MathType::RADIANS:
			{
				if (disp_name == "Value1") {
					return "Degrees";
				}
				else if (disp_name == "Value2") {
					return unused;
				}
				else if (disp_name == "Value3") {
					return unused;
				}
				break;
			}
			case csg::MathType::SNAP:
			{
				if (disp_name == "Value1") {
					return "Value";
				}
				else if (disp_name == "Value2") {
					return "Increment";
				}
				else if (disp_name == "Value3") {
					return unused;
				}
				break;
			}
			case csg::MathType::WRAP:
			{
				if (disp_name == "Value1") {
					return "Value";
				}
				else if (disp_name == "Value2") {
					return "Min";
				}
				else if (disp_name == "Value3") {
					return "Max";
				}
				break;
			}
			case csg::MathType::COMPARE:
			{
				if (disp_name == "Value3") {
					return "Epsilon";
				}
				break;
			}
			case csg::MathType::PINGPONG:
			{
				if (disp_name == "Value1") {
					return "Value";
				}
				else if (disp_name == "Value2") {
					return "Scale";
				}
				else if (disp_name == "Value3") {
					return unused;
				}
				break;
			}
			case csg::MathType::SMOOTH_MIN:
			case csg::MathType::SMOOTH_MAX:
			{
				if (disp_name == "Value3") {
					return "Distance";
				}
				break;
			}
			default:
			{
				// In the default case, find (1), etc in the string to determine which slots are used
				const csg::NodeEnumInfo enum_info{ csg::NodeEnumInfo::from(type) };
				const auto option_info{ csg::NodeEnumOptionInfo::from(enum_info.meta_enum(), type_int) };
				if (option_info) {
					const boost::string_view name_view{ option_info->display_name() };
					if (name_view.find("(1)") != boost::string_view::npos) {
						if (disp_name == "Value1") {
							return "Value";
						}
						else if (disp_name == "Value2") {
							return unused;
						}
						else if (disp_name == "Value3") {
							return unused;
						}
					}
					else if (name_view.find("(2)") != boost::string_view::npos) {
						if (disp_name == "Value3") {
							return unused;
						}
					}
				}
			}
			}
		}
		break;
	case csg::NodeType::VECTOR_MATH:
		if (const auto opt_value{ node.slot_value_as<csg::EnumSlotValue>("type") }) {
			const size_t type_int = opt_value->get();
			const csg::VectorMathType type{ static_cast<csg::VectorMathType>(type_int) };
			// Shortcut for a common case, scale will almost always be replaced here
			if (type != csg::VectorMathType::SCALE && disp_name == "Scale") {
				return unused;
			}
			switch (type) {
			case csg::VectorMathType::SNAP:
			{
				if (disp_name == "Vector1") {
					return "Vector";
				}
				else if (disp_name == "Vector2") {
					return "Increment";
				}
				else if (disp_name == "Vector3") {
					return unused;
				}
				break;
			}
			case csg::VectorMathType::WRAP:
			{
				if (disp_name == "Vector1") {
					return "Vector";
				}
				else if (disp_name == "Vector2") {
					return "Max";
				}
				else if (disp_name == "Vector3") {
					return "Min";
				}
				break;
			}
			default:
			{
				// In the default case, find (1), etc in the string to determine which slots are used
				const csg::NodeEnumInfo enum_info{ csg::NodeEnumInfo::from(type) };
				const auto option_info{ csg::NodeEnumOptionInfo::from(enum_info.meta_enum(), type_int) };
				if (option_info) {
					const boost::string_view name_view{ option_info->display_name() };
					if (name_view.find("(1)") != boost::string_view::npos) {
						if (disp_name == "Vector1") {
							return "Vector";
						}
						else if (disp_name == "Vector2") {
							return unused;
						}
						else if (disp_name == "Vector3") {
							return unused;
						}
					}
					else if (name_view.find("(2)") != boost::string_view::npos) {
						if (disp_name == "Vector3") {
							return unused;
						}
					}
				}
			}
			}
		}
		break;
	case csg::NodeType::TANGENT:
		if (const auto opt_value{ node.slot_value_as<csg::EnumSlotValue>("direction") }) {
			const csg::TangentDirection type{ static_cast<csg::TangentDirection>(opt_value->get()) };
			if (disp_name == "Radial Axis" && type != csg::TangentDirection::RADIAL) {
				return unused;
			}
		}
		break;
	case csg::NodeType::PRINCIPLED_BSDF:
		if (const auto opt_value{ node.slot_value_as<csg::EnumSlotValue>("distribution") }) {
			const csg::PrincipledBSDFDistribution dist{ static_cast<csg::PrincipledBSDFDistribution>(opt_value->get()) };
			if (disp_name == "Transmission Roughness") {
				if (dist == csg::PrincipledBSDFDistribution::GGX) {
					return disp_name;
				}
				else {
					return unused;
				}
			}
		}
		break;
	case csg::NodeType::PRINCIPLED_HAIR:
		if (const auto opt_value{ node.slot_value_as<csg::EnumSlotValue>("coloring") }) {
			const csg::PrincipledHairColoring color{ static_cast<csg::PrincipledHairColoring>(opt_value->get()) };
			if (color != csg::PrincipledHairColoring::ABSORPTION_COEFFICIENT && disp_name == "Absorption Coefficient") {
				return unused;
			}
			else if (color != csg::PrincipledHairColoring::DIRECT_COLORING && disp_name == "Color") {
				return unused;
			}
			else if (color != csg::PrincipledHairColoring::MELANIN_CONCENTRATION) {
				if (disp_name == "Melanin" || disp_name == "Melanin Redness" || disp_name == "Tint" || disp_name == "Random Color") {
					return unused;
				}
			}
		}
		break;
	case csg::NodeType::SUBSURFACE_SCATTER:
		if (const auto opt_value{ node.slot_value_as<csg::EnumSlotValue>("falloff") }) {
			const csg::SubsurfaceScatterFalloff color{ static_cast<csg::SubsurfaceScatterFalloff>(opt_value->get()) };
			if (color != csg::SubsurfaceScatterFalloff::CUBIC && disp_name == "Sharpness") {
				return unused;
			}
		}
		break;
	case csg::NodeType::MUSGRAVE_TEX:
		if (const auto opt_value{ node.slot_value_as<csg::EnumSlotValue>("dimensions") }) {
			const csg::MusgraveTexDimensions dim{ static_cast<csg::MusgraveTexDimensions>(opt_value->get()) };
			if (dim == csg::MusgraveTexDimensions::ONE && disp_name == "Vector") {
				return unused;
			}
			else if ((dim == csg::MusgraveTexDimensions::TWO || dim == csg::MusgraveTexDimensions::THREE) && disp_name == "W") {
				return unused;
			}
		}
		if (const auto opt_value{ node.slot_value_as<csg::EnumSlotValue>("type") }) {
			const csg::MusgraveTexType dim{ static_cast<csg::MusgraveTexType>(opt_value->get()) };
			const std::array<csg::MusgraveTexType, 3>
				with_offset{ csg::MusgraveTexType::RIDGED_MULTIFRACTAL, csg::MusgraveTexType::HYBRID_MULTIFRACTAL, csg::MusgraveTexType::HETERO_TERRAIN };
			const std::array<csg::MusgraveTexType, 2> with_gain{ csg::MusgraveTexType::RIDGED_MULTIFRACTAL, csg::MusgraveTexType::HYBRID_MULTIFRACTAL };
			if (disp_name == "Offset") {
				const bool use_offset{ std::find(with_offset.begin(), with_offset.end(), dim) != with_offset.end() };
				if (use_offset) {
					return disp_name;
				}
				else {
					return unused;
				}
			}
			if (disp_name == "Gain") {
				const bool use_gain{ std::find(with_gain.begin(), with_gain.end(), dim) != with_gain.end() };
				if (use_gain) {
					return disp_name;
				}
				else {
					return unused;
				}
			}
		}
		break;
	case csg::NodeType::NOISE_TEX:
		if (const auto opt_value{ node.slot_value_as<csg::EnumSlotValue>("dimensions") }) {
			const csg::NoiseTexDimensions dim{ static_cast<csg::NoiseTexDimensions>(opt_value->get()) };
			if (dim == csg::NoiseTexDimensions::ONE && disp_name == "Vector") {
				return unused;
			}
			else if ((dim == csg::NoiseTexDimensions::TWO || dim == csg::NoiseTexDimensions::THREE) && disp_name == "W") {
				return unused;
			}
		}
		break;
	case csg::NodeType::VORONOI_TEX:
		if (const auto opt_value{ node.slot_value_as<csg::EnumSlotValue>("dimensions") }) {
			const csg::VoronoiTexDimensions dim{ static_cast<csg::VoronoiTexDimensions>(opt_value->get()) };
			if (dim == csg::VoronoiTexDimensions::ONE && disp_name == "Vector") {
				return unused;
			}
			else if ((dim == csg::VoronoiTexDimensions::TWO || dim == csg::VoronoiTexDimensions::THREE) && disp_name == "W") {
				return unused;
			}
		}
		if (const auto opt_value{ node.slot_value_as<csg::EnumSlotValue>("feature") }) {
			const csg::VoronoiTexFeature feat{ static_cast<csg::VoronoiTexFeature>(opt_value->get()) };
			const std::array<csg::VoronoiTexFeature, 3> with_metric{ csg::VoronoiTexFeature::F1, csg::VoronoiTexFeature::F2, csg::VoronoiTexFeature::SMOOTH_F1 };
			const std::array<csg::VoronoiTexFeature, 3> with_exponent{ csg::VoronoiTexFeature::F1, csg::VoronoiTexFeature::F2, csg::VoronoiTexFeature::SMOOTH_F1 };
			if (disp_name == "Metric") {
				const bool use_metric{ std::find(with_metric.begin(), with_metric.end(), feat) != with_metric.end() };
				if (use_metric) {
					return disp_name;
				}
				else {
					return unused;
				}
			}
			else if (disp_name == "Smoothness") {
				if (feat == csg::VoronoiTexFeature::SMOOTH_F1) {
					return disp_name;
				}
				else {
					return unused;
				}
			}
			else if (disp_name == "Exponent") {
				const bool use_exponent{ std::find(with_exponent.begin(), with_exponent.end(), feat) != with_exponent.end() };
				if (use_exponent) {
					if (const auto opt_metric{ node.slot_value_as<csg::EnumSlotValue>("metric") }) {
						const csg::VoronoiTexMetric metric{ static_cast<csg::VoronoiTexMetric>(opt_metric->get()) };
						if (metric == csg::VoronoiTexMetric::MINKOWSKI) {
							return disp_name;
						}
						else {
							return unused;
						}
					}
				}
				else {
					return unused;
				}
			}
		}
		break;
	case csg::NodeType::WHITE_NOISE_TEX:
		if (const auto opt_value{ node.slot_value_as<csg::EnumSlotValue>("dimensions") }) {
			const csg::WhiteNoiseTexDimensions dim{ static_cast<csg::WhiteNoiseTexDimensions>(opt_value->get()) };
			if (dim == csg::WhiteNoiseTexDimensions::ONE && disp_name == "Vector") {
				return unused;
			}
			else if ((dim == csg::WhiteNoiseTexDimensions::TWO || dim == csg::WhiteNoiseTexDimensions::THREE) && disp_name == "W") {
				return unused;
			}
		}
		break;
	default:
		break;
	}
	return disp_name;
}
