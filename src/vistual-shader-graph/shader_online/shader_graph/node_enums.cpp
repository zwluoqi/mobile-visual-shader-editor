#include "node_enums.h"

#include <utility>

#include <boost/optional.hpp>

template <typename T> static boost::optional<T> as_enum(const size_t option)
{
	if (option >= static_cast<size_t>(T::COUNT)) {
		return boost::none;
	}
	return static_cast<T>(option);
}

template <typename T> static size_t get_count()
{
	return static_cast<size_t>(T::COUNT);
}

boost::optional<csg::NodeEnumInfo> csg::NodeEnumInfo::from(const NodeMetaEnum meta_enum)
{
	if (meta_enum >= NodeMetaEnum::COUNT) {
		return boost::none;
	}

	switch (meta_enum) {
	// Color
	case NodeMetaEnum::MIX_RGB_TYPE:
		return NodeEnumInfo{ meta_enum, get_count<MixRGBType>() };
	// Converter
	case NodeMetaEnum::CLAMP_TYPE:
		return NodeEnumInfo{ meta_enum, get_count<ClampType>() };
	case NodeMetaEnum::MAP_RANGE_TYPE:
		return NodeEnumInfo{ meta_enum, get_count<MapRangeType>() };
	case NodeMetaEnum::MATH_TYPE:
		return NodeEnumInfo{ meta_enum, get_count<MathType>() };
	case NodeMetaEnum::VECTOR_MATH_TYPE:
		return NodeEnumInfo{ meta_enum, get_count<VectorMathType>() };
	// Input
	case NodeMetaEnum::TANGENT_DIRECTION:
		return NodeEnumInfo{ meta_enum, get_count<TangentDirection>() };
	case NodeMetaEnum::TANGENT_AXIS:
		return NodeEnumInfo{ meta_enum, get_count<TangentAxis>() };
	// Shader
	case NodeMetaEnum::ANISOTROPIC_DISTRIBUTION:
		return NodeEnumInfo{ meta_enum, get_count<AnisotropicDistribution>() };
	case NodeMetaEnum::GLASS_DISTRIBUTION:
		return NodeEnumInfo{ meta_enum, get_count<GlassDistribution>() };
	case NodeMetaEnum::GLOSSY_DISTRIBUTION:
		return NodeEnumInfo{ meta_enum, get_count<GlossyDistribution>() };
	case NodeMetaEnum::HAIR_COMPONENT:
		return NodeEnumInfo{ meta_enum, get_count<HairComponent>() };
	case NodeMetaEnum::PRINCIPLED_BSDF_DISTRIBUTION:
		return NodeEnumInfo{ meta_enum, get_count<PrincipledBSDFDistribution>() };
	case NodeMetaEnum::PRINCIPLED_BSDF_SSS:
		return NodeEnumInfo{ meta_enum, get_count<PrincipledBSDFSubsurfaceMethod>() };
	case NodeMetaEnum::PRINCIPLED_HAIR_COLORING:
		return NodeEnumInfo{ meta_enum, get_count<PrincipledHairColoring>() };
	case NodeMetaEnum::REFRACTION_DISTRIBUTION:
		return NodeEnumInfo{ meta_enum, get_count<RefractionDistribution>() };
	case NodeMetaEnum::SSS_FALLOFF:
		return NodeEnumInfo{ meta_enum, get_count<SubsurfaceScatterFalloff>() };
	case NodeMetaEnum::TOON_COMPONENT:
		return NodeEnumInfo{ meta_enum, get_count<ToonComponent>() };
	// Texture
	case NodeMetaEnum::MAX_TEXMAP_PRECISION:
		return NodeEnumInfo{ meta_enum, get_count<MaxTexmapPrecision>() };
	case NodeMetaEnum::GRADIENT_TEX_TYPE:
		return NodeEnumInfo{ meta_enum, get_count<GradientTexType>() };
    case NodeMetaEnum::IMAGE_TEX_InterpolationType_TYPE:
        return NodeEnumInfo{ meta_enum, get_count<ImageTexInterpolationType>() };
    case NodeMetaEnum::IMAGE_TEX_Projection_TYPE:
        return NodeEnumInfo{ meta_enum, get_count<ImageTexProjectionType>() };
	case NodeMetaEnum::MUSGRAVE_TEX_DIMENSIONS:
		return NodeEnumInfo{ meta_enum, get_count<MusgraveTexDimensions>() };
	case NodeMetaEnum::MUSGRAVE_TEX_TYPE:
		return NodeEnumInfo{ meta_enum, get_count<MusgraveTexType>() };
	case NodeMetaEnum::NOISE_TEX_DIMENSIONS:
		return NodeEnumInfo{ meta_enum, get_count<NoiseTexDimensions>() };
	case NodeMetaEnum::VORONOI_TEX_DIMENSIONS:
		return NodeEnumInfo{ meta_enum, get_count<VoronoiTexDimensions>() };
	case NodeMetaEnum::VORONOI_TEX_FEATURE:
		return NodeEnumInfo{ meta_enum, get_count<VoronoiTexFeature>() };
	case NodeMetaEnum::VORONOI_TEX_METRIC:
		return NodeEnumInfo{ meta_enum, get_count<VoronoiTexMetric>() };
	case NodeMetaEnum::WAVE_TEX_TYPE:
		return NodeEnumInfo{ meta_enum, get_count<WaveTexType>() };
	case NodeMetaEnum::WAVE_TEX_DIRECTION:
		return NodeEnumInfo{ meta_enum, get_count<WaveTexDirection>() };
	case NodeMetaEnum::WAVE_TEX_PROFILE:
		return NodeEnumInfo{ meta_enum, get_count<WaveTexProfile>() };
	case NodeMetaEnum::WHITE_NOISE_TEX_DIMENSIONS:
		return NodeEnumInfo{ meta_enum, get_count<WhiteNoiseTexDimensions>() };
	// Vector
	case NodeMetaEnum::DISPLACEMENT_SPACE:
		return NodeEnumInfo{ meta_enum, get_count<DisplacementSpace>() };
	case NodeMetaEnum::VECTOR_MAPPING_TYPE:
		return NodeEnumInfo{ meta_enum, get_count<VectorMappingType>() };
	case NodeMetaEnum::NORMAL_MAP_SPACE:
		return NodeEnumInfo{ meta_enum, get_count<NormalMapSpace>() };
	case NodeMetaEnum::VECTOR_DISPLACEMENT_SPACE:
		return NodeEnumInfo{ meta_enum, get_count<VectorDisplacementSpace>() };
	case NodeMetaEnum::VECTOR_TRANSFORM_TYPE:
		return NodeEnumInfo{ meta_enum, get_count<VectorTransformType>() };
	case NodeMetaEnum::VECTOR_TRANSFORM_SPACE:
		return NodeEnumInfo{ meta_enum, get_count<VectorTransformSpace>() };
	default:
		return boost::none;
	}
}

csg::NodeEnumInfo csg::NodeEnumInfo::from_assert(const NodeMetaEnum meta_enum)
{
	const auto result{ from(meta_enum) };
	assert(result.has_value());
	return *result;
}

boost::optional<csg::NodeEnumOptionInfo> csg::NodeEnumOptionInfo::from(NodeMetaEnum meta_enum, size_t option)
{
	const boost::optional<NodeEnumInfo> enum_info{ NodeEnumInfo::from(meta_enum) };
	if (enum_info.has_value() == false) {
		return boost::none;
	}
	if (option >= enum_info->count()) {
		return boost::none;
	}

	return NodeEnumOptionInfo{ meta_enum, option };
}

struct NameHolder {
	NameHolder(const char* disp_name, const char* internal_name, const char* alt_name = nullptr) :
		disp_name{ disp_name }, internal_name{ internal_name }, alt_name{ alt_name }
	{}

	const char* const disp_name;
	const char* const internal_name;
	const char* const alt_name;
};

// Tiny macro to make it easier to copy paste all these definitions
#define CASE_NAMES(ENUM, DISP, INT) case ENUM: return NameHolder{ DISP, INT };
#define CASE_NAMES_ALT(ENUM, DISP, INT, ALT) case ENUM: return NameHolder{ DISP, INT, ALT };

static NameHolder get_option_names(csg::NodeMetaEnum meta_enum, size_t option)
{
	using namespace csg;

	switch (meta_enum) {

	//////
	// Color
	//////
	case NodeMetaEnum::MIX_RGB_TYPE:
	{
		const auto opt_enum{ as_enum<MixRGBType>(option) };
		if (opt_enum) {
			switch (*opt_enum) {
				CASE_NAMES(MixRGBType::MIX,          "Mix",          "mix_blend");
				CASE_NAMES(MixRGBType::DARKEN,       "Darken",       "mix_darken");
				CASE_NAMES(MixRGBType::MULTIPLY,     "Multiply",     "mix_multiply");
				CASE_NAMES(MixRGBType::BURN,         "Burn",         "mix_burn");
				CASE_NAMES(MixRGBType::LIGHTEN,      "Lighten",      "mix_lighten");
				CASE_NAMES(MixRGBType::SCREEN,       "Screen",       "mix_screen");
				CASE_NAMES(MixRGBType::DODGE,        "Dodge",        "mix_dodge");
				CASE_NAMES(MixRGBType::ADD,          "Add",          "mix_add");
				CASE_NAMES(MixRGBType::OVERLAY,      "Overlay",      "mix_overlay");
				CASE_NAMES(MixRGBType::SOFT_LIGHT,   "Soft Light",   "mix_soft_light");
				CASE_NAMES(MixRGBType::LINEAR_LIGHT, "Linear Light", "mix_linear_light");
				CASE_NAMES(MixRGBType::DIFFERENCE,   "Difference",   "mix_difference");
				CASE_NAMES(MixRGBType::SUBTRACT,     "Subtract",     "mix_subtract");
				CASE_NAMES(MixRGBType::DIVIDE,       "Divide",       "mix_divide");
				CASE_NAMES(MixRGBType::HUE,          "Hue",          "mix_hue");
				CASE_NAMES(MixRGBType::SATURATION,   "Saturation",   "mix_saturation");
				CASE_NAMES(MixRGBType::COLOR,        "Color",        "mix_color");
				CASE_NAMES(MixRGBType::VALUE,        "Value",        "mix_value");
			default:
				return NameHolder{ "[Unknown Type]", "ERROR" };
			}
		}
		else {
			return NameHolder{ "[Bad Type]", "ERROR" };
		}
	}
	//////
	// Converter
	//////
	case NodeMetaEnum::CLAMP_TYPE:
	{
		const auto opt_enum{ as_enum<ClampType>(option) };
		if (opt_enum) {
			switch (*opt_enum) {
				CASE_NAMES(ClampType::Clamp, "Clamp", "clamp_value");
				CASE_NAMES(ClampType::RANGE,  "Range",   "clamp_range");
                CASE_NAMES(ClampType::MINMAX,  "MinMax",   "clamp_minmax");
			default:
				return NameHolder{ "[Unknown Type]", "ERROR" };
			}
		}
		else {
			return NameHolder{ "[Bad Type]", "ERROR" };
		}
	}
	case NodeMetaEnum::MAP_RANGE_TYPE:
	{
		const auto opt_enum{ as_enum<MapRangeType>(option) };
		if (opt_enum) {
			switch (*opt_enum) {
				CASE_NAMES(MapRangeType::LINEAR,        "Linear",        "map_range_linear");
				CASE_NAMES(MapRangeType::STEPPED,       "Stepped",       "map_range_stepped");
				CASE_NAMES(MapRangeType::SMOOTH_STEP,   "Smooth Step",   "map_range_smoothstep");
				CASE_NAMES(MapRangeType::SMOOTHER_STEP, "Smoother Step", "map_range_smootherstep");
			default:
				return NameHolder{ "[Unknown Type]", "ERROR" };
			}
		}
		else {
			return NameHolder{ "[Bad Type]", "ERROR" };
		}
	}
	case NodeMetaEnum::MATH_TYPE:
	{
		const auto opt_enum{ as_enum<MathType>(option) };
		if (opt_enum) {
			switch (*opt_enum) {
				CASE_NAMES(MathType::ADD,          "Add (2)",                "math_add");
				CASE_NAMES(MathType::SUBTRACT,     "Subtract (2)",           "math_subtract");
				CASE_NAMES(MathType::MULTIPLY,     "Multiply (2)",           "math_multiply");
				CASE_NAMES(MathType::DIVIDE,       "Divide (2)",             "math_divide");
				CASE_NAMES(MathType::MULTIPLY_ADD, "Multiply Add (2)",       "math_multiply_add");
				CASE_NAMES(MathType::SINE,         "Sine (1)",               "math_sine");
				CASE_NAMES(MathType::COSINE,       "Cosine (1)",             "math_cosine");
				CASE_NAMES(MathType::TANGENT,      "Tangent (1)",            "math_tangent");
				CASE_NAMES(MathType::ARCSINE,      "Arcsine (1)",            "math_arcsine");
				CASE_NAMES(MathType::ARCCOSINE,    "Arccosine (1)",          "math_arccosine");
				CASE_NAMES(MathType::ARCTANGENT,   "Arctangent (1)",         "math_arctangent");
				CASE_NAMES(MathType::ARCTAN2,      "Arctan2 (2)",            "math_arctan2");
				CASE_NAMES(MathType::SINH,         "Hyperbolic Sine (1)",    "math_sinh");
				CASE_NAMES(MathType::COSH,         "Hyperbolic Cosine (1)",  "math_cosh");
				CASE_NAMES(MathType::TANH,         "Hyperbolic Tangent (1)", "math_tanh");
				CASE_NAMES(MathType::POWER,        "Power (2)",              "math_power");
				CASE_NAMES(MathType::LOGARITHM,    "Logarithm (2)",          "math_logarithm");
				CASE_NAMES(MathType::MINIMUM,      "Maximum (2)",            "math_maximum");
				CASE_NAMES(MathType::MAXIMUM,      "Minimum (2)",            "math_minimum");
				CASE_NAMES(MathType::LESS_THAN,    "Less Than (2)",          "math_less_than");
				CASE_NAMES(MathType::GREATER_THAN, "Greater Than (2)",       "math_greater_than");
				CASE_NAMES(MathType::MODULO,       "Modulo (2)",             "math_modulo");
				CASE_NAMES(MathType::ABSOLUTE,     "Absolute (1)",           "math_absolute");
				CASE_NAMES(MathType::ROUND,        "Round (1)",              "math_round");
				CASE_NAMES(MathType::FLOOR,        "Floor (1)",              "math_floor");
				CASE_NAMES(MathType::CEIL,         "Ceil (1)",               "math_ceil");
				CASE_NAMES(MathType::FRACTION,     "Fraction (1)",           "math_fraction");
				CASE_NAMES(MathType::SQRT,         "Square Root (1)",        "math_sqrt");
				CASE_NAMES(MathType::INV_SQRT,     "Inverse Sqrt (1)",       "math_inverse_sqrt");
				CASE_NAMES(MathType::SIGN,         "Sign (1)",               "math_sign");
				CASE_NAMES(MathType::EXPONENT,     "Exponent (1)",           "math_exponent");
				CASE_NAMES(MathType::RADIANS,      "To Radians (1)",         "math_radians");
				CASE_NAMES(MathType::DEGREES,      "To Degrees (1)",         "math_degrees");
				CASE_NAMES(MathType::TRUNC,        "Truncate (1)",           "math_truncate");
				CASE_NAMES(MathType::SNAP,         "Snap (2)",               "math_snap");
				CASE_NAMES(MathType::WRAP,         "Wrap (3)",               "math_wrap");
				CASE_NAMES(MathType::COMPARE,      "Compare (3)",            "math_compare");
				CASE_NAMES(MathType::PINGPONG,     "Ping Pong (3)",          "math_pingpong");
				CASE_NAMES(MathType::SMOOTH_MIN,   "Smooth Min (3)",         "math_smooth_min");
				CASE_NAMES(MathType::SMOOTH_MAX,   "Smooth Max (3)",         "math_smooth_max");
			default:
				return NameHolder{ "[Unknown Type]", "ERROR" };
			}
		}
		else {
			return NameHolder{ "[Bad Type]", "ERROR" };
		}
	}
	case NodeMetaEnum::VECTOR_MATH_TYPE:
	{
		const auto opt_enum{ as_enum<VectorMathType>(option) };
		if (opt_enum) {
			switch (*opt_enum) {
				CASE_NAMES(VectorMathType::ADD,           "Add (2)",           "vector_math_add");
				CASE_NAMES(VectorMathType::SUBTRACT,      "Subtract (2)",      "vector_math_subtract");
				CASE_NAMES(VectorMathType::MULTIPLY,      "Multiply (2)",      "vector_math_multiply");
				CASE_NAMES(VectorMathType::DIVIDE,        "Divide (2)",        "vector_math_divide");
				CASE_NAMES(VectorMathType::CROSS_PRODUCT, "Cross Product (2)", "vector_math_cross");
				CASE_NAMES(VectorMathType::PROJECT,       "Project (2)",       "vector_math_project");
				CASE_NAMES(VectorMathType::REFLECT,       "Reflect (2)",       "vector_math_reflect");
				CASE_NAMES(VectorMathType::DOT_PRODUCT,   "Dot Product (2)",   "vector_math_dot");
				CASE_NAMES(VectorMathType::DISTANCE,      "Distance (2)",      "vector_math_distance");
				CASE_NAMES(VectorMathType::LENGTH,        "Length (1)",        "vector_math_length");
				CASE_NAMES(VectorMathType::SCALE,         "Scale (1)",         "vector_math_scale");
				CASE_NAMES(VectorMathType::NORMALIZE,     "Normalize (1)",     "vector_math_normalize");
				CASE_NAMES(VectorMathType::SNAP,          "Snap (2)",          "vector_math_snap");
				CASE_NAMES(VectorMathType::FLOOR,         "Floor (1)",         "vector_math_floor");
				CASE_NAMES(VectorMathType::CEIL,          "Ceil (1)",          "vector_math_ceil");
				CASE_NAMES(VectorMathType::MODULO,        "Modulo (2)",        "vector_math_modulo");
				CASE_NAMES(VectorMathType::FRACTION,      "Fraction (1)",      "vector_math_fraction");
				CASE_NAMES(VectorMathType::ABSOLUTE,      "Absolute (1)",      "vector_math_absolute");
				CASE_NAMES(VectorMathType::MINIMUM,       "Minimum (2)",       "vector_math_minimum");
				CASE_NAMES(VectorMathType::MAXIMUM,       "Maximum (2)",       "vector_math_maximum");
				CASE_NAMES(VectorMathType::WRAP,          "Wrap (3)",          "wrap");
				CASE_NAMES(VectorMathType::SINE,          "Sine (1)",          "vector_math_sine");
				CASE_NAMES(VectorMathType::COSINE,        "Cosine (1)",        "vector_math_cosine");
				CASE_NAMES(VectorMathType::TANGENT,       "Tangent (1)",       "vector_math_tangent");
			default:
				return NameHolder{ "[Unknown Type]", "ERROR" };
			}
		}
		else {
			return NameHolder{ "[Bad Type]", "ERROR" };
		}
	}
	//////
	// Input
	//////
	case NodeMetaEnum::TANGENT_DIRECTION:
	{
		const auto opt_enum{ as_enum<TangentDirection>(option) };
		if (opt_enum) {
			switch (*opt_enum) {
				CASE_NAMES(TangentDirection::RADIAL, "Radial", "radial");
				CASE_NAMES(TangentDirection::UV_MAP, "UV Map", "uv_map");
			default:
				return NameHolder{ "[Unknown Direction]", "ERROR" };
			}
		}
		else {
			return NameHolder{ "[Bad Direction]", "ERROR" };
		}
	}
	case NodeMetaEnum::TANGENT_AXIS:
	{
		const auto opt_enum{ as_enum<TangentAxis>(option) };
		if (opt_enum) {
			switch (*opt_enum) {
				CASE_NAMES(TangentAxis::X, "X", "x");
				CASE_NAMES(TangentAxis::Y, "Y", "y");
				CASE_NAMES(TangentAxis::Z, "Z", "z");
			default:
				return NameHolder{ "[Unknown Axis]", "ERROR" };
			}
		}
		else {
			return NameHolder{ "[Bad Axis]", "ERROR" };
		}
	}
	//////
	// Shader
	//////
	case NodeMetaEnum::ANISOTROPIC_DISTRIBUTION:
	{
		const auto opt_enum{ as_enum<AnisotropicDistribution>(option) };
		if (opt_enum) {
			switch (*opt_enum) {
				CASE_NAMES(AnisotropicDistribution::ASHIKHMIN_SHIRLEY, "Ashikhmin-Shirley", "ashikhmin_shirley");
				CASE_NAMES(AnisotropicDistribution::BECKMANN,          "Beckmann",          "beckmann");
				CASE_NAMES(AnisotropicDistribution::GGX,               "GGX",               "ggx");
				CASE_NAMES_ALT(AnisotropicDistribution::MULTISCATTER_GGX, "Multiscatter GGX", "multi_ggx", "multiscatter_ggx");
			default:
				return NameHolder{ "[Unknown Distribution]", "ERROR" };
			}
		}
		else {
			return NameHolder{ "[Bad Distribution]", "ERROR" };
		}
	}
	case NodeMetaEnum::GLASS_DISTRIBUTION:
	{
		const auto opt_enum{ as_enum<GlassDistribution>(option) };
		if (opt_enum) {
			switch (*opt_enum) {
				CASE_NAMES(GlassDistribution::GGX,              "GGX",              "ggx");
				CASE_NAMES(GlassDistribution::BECKMANN,         "Beckmann",         "beckmann");
				CASE_NAMES(GlassDistribution::SHARP,            "Sharp",            "sharp");
				CASE_NAMES_ALT(GlassDistribution::MULTISCATTER_GGX, "Multiscatter GGX", "multi_ggx", "multiscatter_ggx");
			default:
				return NameHolder{ "[Unknown Distribution]", "ERROR" };
			}
		}
		else {
			return NameHolder{ "[Bad Distribution]", "ERROR" };
		}
	}
	case NodeMetaEnum::GLOSSY_DISTRIBUTION:
	{
		const auto opt_enum{ as_enum<GlossyDistribution>(option) };
		if (opt_enum) {
			switch (*opt_enum) {
				CASE_NAMES(GlossyDistribution::ASHIKHMIN_SHIRLEY, "Ashikhmin-Shirley", "ashikhmin_shirley");
				CASE_NAMES(GlossyDistribution::BECKMANN,          "Beckmann",          "beckmann");
				CASE_NAMES(GlossyDistribution::GGX,               "GGX",               "ggx");
				CASE_NAMES(GlossyDistribution::SHARP,             "Sharp",             "sharp");
				CASE_NAMES_ALT(GlossyDistribution::MULTISCATTER_GGX, "Multiscatter GGX", "multi_ggx", "multiscatter_ggx");
			default:
				return NameHolder{ "[Unknown Distribution]", "ERROR" };
			}
		}
		else {
			return NameHolder{ "[Bad Distribution]", "ERROR" };
		}
	}
	case NodeMetaEnum::HAIR_COMPONENT:
	{
		const auto opt_enum{ as_enum<HairComponent>(option) };
		if (opt_enum) {
			switch (*opt_enum) {
				CASE_NAMES(HairComponent::REFLECTION,   "Reflection",   "reflection");
				CASE_NAMES(HairComponent::TRANSMISSION, "Transmission", "transmission");
			default:
				return NameHolder{ "[Unknown Distribution]", "ERROR" };
			}
		}
		else {
			return NameHolder{ "[Bad Distribution]", "ERROR" };
		}
	}
	case NodeMetaEnum::PRINCIPLED_BSDF_DISTRIBUTION:
	{
		const auto opt_enum{ as_enum<PrincipledBSDFDistribution>(option) };
		if (opt_enum) {
			switch (*opt_enum) {
				CASE_NAMES(PrincipledBSDFDistribution::GGX,              "GGX",              "ggx");
				CASE_NAMES_ALT(PrincipledBSDFDistribution::MULTISCATTER_GGX, "Multiscatter GGX", "multi_ggx", "multiscatter_ggx");
			default:
				return NameHolder{ "[Unknown Distribution]", "ERROR" };
			}
		}
		else {
			return NameHolder{ "[Bad Distribution]", "ERROR" };
		}
	}
	case NodeMetaEnum::PRINCIPLED_BSDF_SSS:
	{
		const auto opt_enum{ as_enum<PrincipledBSDFSubsurfaceMethod>(option) };
		if (opt_enum) {
			switch (*opt_enum) {
				CASE_NAMES(PrincipledBSDFSubsurfaceMethod::BURLEY,      "Burley",      "burley");
				CASE_NAMES(PrincipledBSDFSubsurfaceMethod::RANDOM_WALK, "Random Walk", "random_walk");
			default:
				return NameHolder{ "[Unknown Method]", "ERROR" };
			}
		}
		else {
			return NameHolder{ "[Bad Method]", "ERROR" };
		}
	}
	case NodeMetaEnum::PRINCIPLED_HAIR_COLORING:
	{
		const auto opt_enum{ as_enum<PrincipledHairColoring>(option) };
		if (opt_enum) {
			switch (*opt_enum) {
				CASE_NAMES_ALT(PrincipledHairColoring::ABSORPTION_COEFFICIENT, "Absorption coefficient", "absorption", "absorption_coefficient");
				CASE_NAMES_ALT(PrincipledHairColoring::MELANIN_CONCENTRATION,  "Melanin concentration",  "melanin",    "melanin_concentration");
				CASE_NAMES_ALT(PrincipledHairColoring::DIRECT_COLORING,        "Direct coloring",        "color",      "direct_coloring");
			default:
				return NameHolder{ "[Unknown Coloring]", "ERROR" };
			}
		}
		else {
			return NameHolder{ "[Bad Coloring]", "ERROR" };
		}
	}
	case NodeMetaEnum::REFRACTION_DISTRIBUTION:
	{
		const auto opt_enum{ as_enum<RefractionDistribution>(option) };
		if (opt_enum) {
			switch (*opt_enum) {
				CASE_NAMES(RefractionDistribution::BECKMANN, "Beckmann", "beckmann");
				CASE_NAMES(RefractionDistribution::GGX,      "GGX",      "ggx");
				CASE_NAMES(RefractionDistribution::SHARP,    "Sharp",    "sharp");
			default:
				return NameHolder{ "[Unknown Distribution]", "ERROR" };
			}
		}
		else {
			return NameHolder{ "[Bad Distribution]", "ERROR" };
		}
	}
	case NodeMetaEnum::SSS_FALLOFF:
	{
		const auto opt_enum{ as_enum<SubsurfaceScatterFalloff>(option) };
		if (opt_enum) {
			switch (*opt_enum) {
				CASE_NAMES(SubsurfaceScatterFalloff::BURLEY,      "Burley",      "burley");
				CASE_NAMES(SubsurfaceScatterFalloff::CUBIC,       "Cubic",       "cubic");
				CASE_NAMES(SubsurfaceScatterFalloff::GAUSSIAN,    "Gaussian",    "gaussian");
				CASE_NAMES(SubsurfaceScatterFalloff::RANDOM_WALK, "Random Walk", "random_walk");
			default:
				return NameHolder{ "[Unknown Falloff]", "ERROR" };
			}
		}
		else {
			return NameHolder{ "[Bad Falloff]", "ERROR" };
		}
	}
	case NodeMetaEnum::TOON_COMPONENT:
	{
		const auto opt_enum{ as_enum<ToonComponent>(option) };
		if (opt_enum) {
			switch (*opt_enum) {
				CASE_NAMES(ToonComponent::DIFFUSE, "Diffuse", "diffuse");
				CASE_NAMES(ToonComponent::GLOSSY,  "Glossy",  "glossy");
			default:
				return NameHolder{ "[Unknown Component]", "ERROR" };
			}
		}
		else {
			return NameHolder{ "[Bad Component]", "ERROR" };
		}
	}
	//////
	// Texmap
	//////
	case NodeMetaEnum::MAX_TEXMAP_PRECISION:
	{
		const auto opt_enum{ as_enum<MaxTexmapPrecision>(option) };
		if (opt_enum) {
			switch (*opt_enum) {
				CASE_NAMES(MaxTexmapPrecision::UCHAR, "8-bit/Channel Char",   "uchar");
				CASE_NAMES(MaxTexmapPrecision::FLOAT, "32-bit/Channel Float", "float");
			default:
				return NameHolder{ "[Unknown Precision]", "ERROR" };
			}
		}
		else {
			return NameHolder{ "[Bad Precision]", "ERROR" };
		}
	}
	case NodeMetaEnum::GRADIENT_TEX_TYPE:
	{
		const auto opt_enum{ as_enum<GradientTexType>(option) };
		if (opt_enum) {
			switch (*opt_enum) {
				CASE_NAMES(GradientTexType::LINEAR,           "Linear",           "node_tex_gradient_linear");
				CASE_NAMES(GradientTexType::QUADRATIC,        "Quadratic",        "node_tex_gradient_quadratic");
				CASE_NAMES(GradientTexType::EASING,           "Easing",           "node_tex_gradient_easing");
				CASE_NAMES(GradientTexType::DIAGONAL,         "Diagonal",         "node_tex_gradient_diagonal");
				CASE_NAMES(GradientTexType::RADIAL,           "Radial",           "node_tex_gradient_radial");
				CASE_NAMES(GradientTexType::QUADRATIC_SPHERE, "Quadratic Shpere", "node_tex_gradient_quadratic_sphere");
				CASE_NAMES(GradientTexType::SPHERICAL,        "Shperical",        "node_tex_gradient_spherical");
			default:
				return NameHolder{ "[Unknown Type]", "ERROR" };
			}
		}
		else {
			return NameHolder{ "[Bad Type]", "ERROR" };
		}
	}
    case NodeMetaEnum::IMAGE_TEX_InterpolationType_TYPE:
    {
        const auto opt_enum{ as_enum<ImageTexInterpolationType>(option) };
        if (opt_enum) {
            switch (*opt_enum) {
                CASE_NAMES(ImageTexInterpolationType::SHD_INTERP_LINEAR,           "LINEAR",           "node_tex_image_linear_simple");
                CASE_NAMES(ImageTexInterpolationType::SHD_INTERP_CLOSEST,        "CLOSEST",        "node_tex_image_linear_simple");
                CASE_NAMES(ImageTexInterpolationType::SHD_INTERP_CUBIC,           "CUBIC",           "node_tex_image_cubic_simple");
                CASE_NAMES(ImageTexInterpolationType::SHD_INTERP_SMART,         "SMART",         "node_tex_image_cubic_simple");
                    
            default:
                return NameHolder{ "[Unknown Type]", "ERROR" };
            }
        }
        else {
            return NameHolder{ "[Bad Type]", "ERROR" };
        }
    }
    case NodeMetaEnum::IMAGE_TEX_Projection_TYPE:
    {
        const auto opt_enum{ as_enum<ImageTexProjectionType>(option) };
        if (opt_enum) {
            switch (*opt_enum) {
                CASE_NAMES(ImageTexProjectionType::SHD_PROJ_FLAT,           "FLAT",           "SHD_PROJ_FLAT");
                CASE_NAMES(ImageTexProjectionType::SHD_PROJ_BOX,        "BOX",        "SHD_PROJ_BOX");
                CASE_NAMES(ImageTexProjectionType::SHD_PROJ_SPHERE,           "SPHERE",           "SHD_PROJ_SPHERE");
                CASE_NAMES(ImageTexProjectionType::SHD_PROJ_TUBE,         "TUBE",         "SHD_PROJ_TUBE");
            default:
                return NameHolder{ "[Unknown Type]", "ERROR" };
            }
        }
        else {
            return NameHolder{ "[Bad Type]", "ERROR" };
        }
    }
	case NodeMetaEnum::MUSGRAVE_TEX_DIMENSIONS:
	{
		const auto opt_enum{ as_enum<MusgraveTexDimensions>(option) };
		if (opt_enum) {
			switch (*opt_enum) {
				CASE_NAMES(MusgraveTexDimensions::ONE,   "1D", "1d");
				CASE_NAMES(MusgraveTexDimensions::TWO,   "2D", "2d");
				CASE_NAMES(MusgraveTexDimensions::THREE, "3D", "3d");
				CASE_NAMES(MusgraveTexDimensions::FOUR,  "4D", "4d");
			default:
				return NameHolder{ "[Unknown Dimensions]", "ERROR" };
			}
		}
		else {
			return NameHolder{ "[Bad Dimensions]", "ERROR" };
		}
	}
	case NodeMetaEnum::MUSGRAVE_TEX_TYPE:
	{
		const auto opt_enum{ as_enum<MusgraveTexType>(option) };
		if (opt_enum) {
			switch (*opt_enum) {
				CASE_NAMES(MusgraveTexType::MULTIFRACTAL,        "Multifractal",        "node_tex_musgrave_multi_fractal");
				CASE_NAMES(MusgraveTexType::RIDGED_MULTIFRACTAL, "Ridged Multifractal", "node_tex_musgrave_ridged_multi_fractal");
				CASE_NAMES(MusgraveTexType::HYBRID_MULTIFRACTAL, "Hyvrid Multifractal", "node_tex_musgrave_hybrid_multi_fractal");
				CASE_NAMES(MusgraveTexType::FBM,                 "fBM",                 "node_tex_musgrave_fBm");
				CASE_NAMES(MusgraveTexType::HETERO_TERRAIN,      "Hetero Terrain",      "node_tex_musgrave_hetero_terrain");
			default:
				return NameHolder{ "[Unknown Type]", "ERROR" };
			}
		}
		else {
			return NameHolder{ "[Bad Type]", "ERROR" };
		}
	}
	case NodeMetaEnum::NOISE_TEX_DIMENSIONS:
	{
		const auto opt_enum{ as_enum<NoiseTexDimensions>(option) };
		if (opt_enum) {
			switch (*opt_enum) {
				CASE_NAMES(NoiseTexDimensions::ONE,   "1D", "node_noise_texture_1d");
				CASE_NAMES(NoiseTexDimensions::TWO,   "2D", "node_noise_texture_2d");
				CASE_NAMES(NoiseTexDimensions::THREE, "3D", "node_noise_texture_3d");
				CASE_NAMES(NoiseTexDimensions::FOUR,  "4D", "node_noise_texture_4d");
			default:
				return NameHolder{ "[Unknown Dimensions]", "ERROR" };
			}
		}
		else {
			return NameHolder{ "[Bad Dimensions]", "ERROR" };
		}
	}
	case NodeMetaEnum::VORONOI_TEX_DIMENSIONS:
	{
		const auto opt_enum{ as_enum<VoronoiTexDimensions>(option) };
		if (opt_enum) {
			switch (*opt_enum) {
				CASE_NAMES(VoronoiTexDimensions::ONE,   "1D", "1d");
				CASE_NAMES(VoronoiTexDimensions::TWO,   "2D", "2d");
				CASE_NAMES(VoronoiTexDimensions::THREE, "3D", "3d");
				CASE_NAMES(VoronoiTexDimensions::FOUR,  "4D", "4d");
			default:
				return NameHolder{ "[Unknown Dimensions]", "ERROR" };
			}
		}
		else {
			return NameHolder{ "[Bad Dimensions]", "ERROR" };
		}
	}
	case NodeMetaEnum::VORONOI_TEX_FEATURE:
	{
		const auto opt_enum{ as_enum<VoronoiTexFeature>(option) };
		if (opt_enum) {
			switch (*opt_enum) {
				CASE_NAMES(VoronoiTexFeature::F1, "F1", "f1");
				CASE_NAMES(VoronoiTexFeature::F2, "F2", "f2");
				CASE_NAMES(VoronoiTexFeature::SMOOTH_F1, "Smooth F1", "smooth_f1");
				CASE_NAMES(VoronoiTexFeature::DISTANCE_TO_EDGE, "Distance to Edge", "distance_to_edge");
				CASE_NAMES(VoronoiTexFeature::N_SPHERE_RADIUS, "N-Sphere Radius", "n_sphere_radius");
			default:
				return NameHolder{ "[Unknown Feature]", "ERROR" };
			}
		}
		else {
			return NameHolder{ "[Bad Feature]", "ERROR" };
		}
	}
	case NodeMetaEnum::VORONOI_TEX_METRIC:
	{
		const auto opt_enum{ as_enum<VoronoiTexMetric>(option) };
		if (opt_enum) {
			switch (*opt_enum) {
				CASE_NAMES(VoronoiTexMetric::EUCLIDEAN, "Euclidean", "euclidean");
				CASE_NAMES(VoronoiTexMetric::MANHATTAN, "Manhattan", "manhattan");
				CASE_NAMES(VoronoiTexMetric::CHEBYCHEV, "Chebychev", "chebychev");
				CASE_NAMES(VoronoiTexMetric::MINKOWSKI, "Minkowski", "minkowski");
			default:
				return NameHolder{ "[Unknown Feature]", "ERROR" };
			}
		}
		else {
			return NameHolder{ "[Bad Feature]", "ERROR" };
		}
	}
	case NodeMetaEnum::WAVE_TEX_TYPE:
	{
		const auto opt_enum{ as_enum<WaveTexType>(option) };
		if (opt_enum) {
			switch (*opt_enum) {
				CASE_NAMES(WaveTexType::BANDS, "Bands", "bands");
				CASE_NAMES(WaveTexType::RINGS, "Rings", "rings");
			default:
				return NameHolder{ "[Unknown Type]", "ERROR" };
			}
		}
		else {
			return NameHolder{ "[Bad Type]", "ERROR" };
		}
	}
	case NodeMetaEnum::WAVE_TEX_DIRECTION:
	{
		const auto opt_enum{ as_enum<WaveTexDirection>(option) };
		if (opt_enum) {
			switch (*opt_enum) {
				CASE_NAMES(WaveTexDirection::X, "X", "x");
				CASE_NAMES(WaveTexDirection::Y, "Y", "y");
				CASE_NAMES(WaveTexDirection::Z, "Z", "z");
				CASE_NAMES(WaveTexDirection::DIAGONAL, "Diagonal", "diagonal");
			default:
				return NameHolder{ "[Unknown Direction]", "ERROR" };
			}
		}
		else {
			return NameHolder{ "[Bad Direction]", "ERROR" };
		}
	}
	case NodeMetaEnum::WAVE_TEX_PROFILE:
	{
		const auto opt_enum{ as_enum<WaveTexProfile>(option) };
		if (opt_enum) {
			switch (*opt_enum) {
				CASE_NAMES(WaveTexProfile::SINE,     "Sine",     "sine");
				CASE_NAMES(WaveTexProfile::SAW,      "Saw",      "saw");
				CASE_NAMES(WaveTexProfile::TRIANGLE, "Triangle", "triangle");
			default:
				return NameHolder{ "[Unknown Profile]", "ERROR" };
			}
		}
		else {
			return NameHolder{ "[Bad Profile]", "ERROR" };
		}
	}
	case NodeMetaEnum::WHITE_NOISE_TEX_DIMENSIONS:
	{
		const auto opt_enum{ as_enum<WhiteNoiseTexDimensions>(option) };
		if (opt_enum) {
			switch (*opt_enum) {
				CASE_NAMES(WhiteNoiseTexDimensions::ONE, "1D", "node_white_noise_1d");
				CASE_NAMES(WhiteNoiseTexDimensions::TWO, "2D", "node_white_noise_2d");
				CASE_NAMES(WhiteNoiseTexDimensions::THREE, "3D", "node_white_noise_3d");
				CASE_NAMES(WhiteNoiseTexDimensions::FOUR, "4D", "node_white_noise_4d");
			default:
				return NameHolder{ "[Unknown Dimensions]", "ERROR" };
			}
		}
		else {
			return NameHolder{ "[Bad Dimensions]", "ERROR" };
		}
	}
	//////
	// Vector
	//////
	case NodeMetaEnum::DISPLACEMENT_SPACE:
	{
		const auto opt_enum{ as_enum<DisplacementSpace>(option) };
		if (opt_enum) {
			switch (*opt_enum) {
				CASE_NAMES(DisplacementSpace::OBJECT, "Object", "node_displacement_object");
				CASE_NAMES(DisplacementSpace::WORLD,  "World",  "node_displacement_world");
			default:
				return NameHolder{ "[Unknown Space]", "ERROR" };
			}
		}
		else {
			return NameHolder{ "[Bad Space]", "ERROR" };
		}
	}
	case NodeMetaEnum::VECTOR_MAPPING_TYPE:
	{
		const auto opt_enum{ as_enum<VectorMappingType>(option) };
		if (opt_enum) {
			switch (*opt_enum) {
				CASE_NAMES(VectorMappingType::POINT,   "Point",   "mapping_point");
				CASE_NAMES(VectorMappingType::TEXTURE, "Texture", "mapping_texture");
				CASE_NAMES(VectorMappingType::VECTOR,  "Vector",  "mapping_vector");
				CASE_NAMES(VectorMappingType::NORMAL,  "Normal",  "mapping_normal");
			default:
				return NameHolder{ "[Unknown Type]", "ERROR" };
			}
		}
		else {
			return NameHolder{ "[Bad Type]", "ERROR" };
		}
	}
	case NodeMetaEnum::NORMAL_MAP_SPACE:
	{
		const auto opt_enum{ as_enum<NormalMapSpace>(option) };
		if (opt_enum) {
			switch (*opt_enum) {
				CASE_NAMES(NormalMapSpace::TANGENT, "Tangent", "node_normal_map_tangent");
				CASE_NAMES(NormalMapSpace::OBJECT,  "Object",  "node_normal_map_obj");
				CASE_NAMES(NormalMapSpace::WORLD,   "World",   "node_normal_map_world");
			default:
				return NameHolder{ "[Unknown Space]", "ERROR" };
			}
		}
		else {
			return NameHolder{ "[Bad Space]", "ERROR" };
		}
	}
	case NodeMetaEnum::VECTOR_DISPLACEMENT_SPACE:
	{
		const auto opt_enum{ as_enum<VectorDisplacementSpace>(option) };
		if (opt_enum) {
			switch (*opt_enum) {
				CASE_NAMES(VectorDisplacementSpace::TANGENT, "Tangent", "node_vector_displacement_tangent");
				CASE_NAMES(VectorDisplacementSpace::OBJECT,  "Object",  "node_vector_displacement_object");
				CASE_NAMES(VectorDisplacementSpace::WORLD,   "World",   "node_vector_displacement_world");
			default:
				return NameHolder{ "[Unknown Space]", "ERROR" };
			}
		}
		else {
			return NameHolder{ "[Bad Space]", "ERROR" };
		}
	}
	case NodeMetaEnum::VECTOR_TRANSFORM_TYPE:
	{
		const auto opt_enum{ as_enum<VectorTransformType>(option) };
		if (opt_enum) {
			switch (*opt_enum) {
				CASE_NAMES(VectorTransformType::POINT,  "Point",  "transform_point");
				CASE_NAMES(VectorTransformType::VECTOR, "Vector", "transform_vector");
				CASE_NAMES(VectorTransformType::NORMAL, "Normal", "transform_normal");
			default:
				return NameHolder{ "[Unknown Type]", "ERROR" };
			}
		}
		else {
			return NameHolder{ "[Bad Type]", "ERROR" };
		}
	}
	case NodeMetaEnum::VECTOR_TRANSFORM_SPACE:
	{
		const auto opt_enum{ as_enum<VectorTransformSpace>(option) };
		if (opt_enum) {
			switch (*opt_enum) {
				CASE_NAMES(VectorTransformSpace::CAMERA, "Camera", "camera");
				CASE_NAMES(VectorTransformSpace::OBJECT, "Object", "object");
				CASE_NAMES(VectorTransformSpace::WORLD,  "World",  "world");
			default:
				return NameHolder{ "[Unknown Space]", "ERROR" };
			}
		}
		else {
			return NameHolder{ "[Bad Space]", "ERROR" };
		}
	}
	/*
	case NodeMetaEnum::COPY_PASTE:
	{
		const auto opt_enum{ as_enum<EnumName>(option) };
		if (opt_enum) {
			switch (*opt_enum) {
				CASE_NAMES(Enum::OPTION1, "Option1", "option1");
				CASE_NAMES(Enum::OPTION2, "Option2", "option2");
			default:
				return NameHolder{ "[Unknown]", "ERROR" };
			}
		}
		else {
			return NameHolder{ "[Bad]", "ERROR" };
		}
	}
	*/
	default:
		return NameHolder{ "[Enum Error]", "ERROR" };
	}
}

const char* csg::NodeEnumOptionInfo::display_name() const
{
	return get_option_names(_meta_enum, _option).disp_name;
}

const char* csg::NodeEnumOptionInfo::internal_name() const
{
	return get_option_names(_meta_enum, _option).internal_name;
}

const char* csg::NodeEnumOptionInfo::alt_name() const
{
	return get_option_names(_meta_enum, _option).alt_name;
}
