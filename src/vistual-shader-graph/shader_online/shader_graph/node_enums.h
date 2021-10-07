#pragma once

#include <cstddef>

#include <boost/optional.hpp>

namespace csg {
	enum class NodeMetaEnum {
		// Color
		MIX_RGB_TYPE,
		// Converter
		CLAMP_TYPE,
		MAP_RANGE_TYPE,
		MATH_TYPE,
		VECTOR_MATH_TYPE,
		// Input
		TANGENT_DIRECTION,
		TANGENT_AXIS,
		// Shader
		ANISOTROPIC_DISTRIBUTION,
		GLASS_DISTRIBUTION,
		GLOSSY_DISTRIBUTION,
		HAIR_COMPONENT,
		PRINCIPLED_BSDF_DISTRIBUTION,
		PRINCIPLED_BSDF_SSS,
		PRINCIPLED_HAIR_COLORING,
		REFRACTION_DISTRIBUTION,
		SSS_FALLOFF,
		TOON_COMPONENT,
		// Texture
		MAX_TEXMAP_PRECISION,
		GRADIENT_TEX_TYPE,
        IMAGE_TEX_InterpolationType_TYPE,
        IMAGE_TEX_Projection_TYPE,
		MUSGRAVE_TEX_DIMENSIONS,
		MUSGRAVE_TEX_TYPE,
		NOISE_TEX_DIMENSIONS,
		VORONOI_TEX_DIMENSIONS,
		VORONOI_TEX_FEATURE,
		VORONOI_TEX_METRIC,
		WAVE_TEX_TYPE,
		WAVE_TEX_DIRECTION,
		WAVE_TEX_PROFILE,
		WHITE_NOISE_TEX_DIMENSIONS,
		// Vector
		DISPLACEMENT_SPACE,
		VECTOR_MAPPING_TYPE,
		NORMAL_MAP_SPACE,
		VECTOR_DISPLACEMENT_SPACE,
		VECTOR_TRANSFORM_TYPE,
		VECTOR_TRANSFORM_SPACE,
		// Bookkeeping
		COUNT
	};

	enum class MixRGBType {
		MIX,
		DARKEN,
		MULTIPLY,
		BURN,
		LIGHTEN,
		SCREEN,
		DODGE,
		ADD,
		OVERLAY,
		SOFT_LIGHT,
		LINEAR_LIGHT,
		DIFFERENCE,
		SUBTRACT,
		DIVIDE,
		HUE,
		SATURATION,
		COLOR,
		VALUE,
		COUNT
	};

	enum class ClampType {
		Clamp,
		RANGE,
        MINMAX,
		COUNT
	};

	enum class MapRangeType {
		LINEAR,
		STEPPED,
		SMOOTH_STEP,
		SMOOTHER_STEP,
		COUNT
	};

	enum class MathType {
		ADD,
		SUBTRACT,
		MULTIPLY,
		DIVIDE,
		MULTIPLY_ADD,
		SINE,
		COSINE,
		TANGENT,
		ARCSINE,
		ARCCOSINE,
		ARCTANGENT,
		ARCTAN2,
		SINH,
		COSH,
		TANH,
		POWER,
		LOGARITHM,
		MINIMUM,
		MAXIMUM,
		LESS_THAN,
		GREATER_THAN,
		MODULO,
		ABSOLUTE,
		ROUND,
		FLOOR,
		CEIL,
		FRACTION,
		SQRT,
		INV_SQRT,
		SIGN,
		EXPONENT,
		RADIANS,
		DEGREES,
		TRUNC,
		SNAP,
		WRAP,
		COMPARE,
		PINGPONG,
		SMOOTH_MIN,
		SMOOTH_MAX,
		COUNT
	};

	enum class VectorMathType {
		ADD,
		SUBTRACT,
		MULTIPLY,
		DIVIDE,
		CROSS_PRODUCT,
		PROJECT,
		REFLECT,
		DOT_PRODUCT,
		DISTANCE,
		LENGTH,
		SCALE,
		NORMALIZE,
		SNAP,
		FLOOR,
		CEIL,
		MODULO,
		FRACTION,
		ABSOLUTE,
		MINIMUM,
		MAXIMUM,
		WRAP,
		SINE,
		COSINE,
		TANGENT,
		COUNT
	};

	enum class TangentDirection {
		RADIAL,
		UV_MAP,
		COUNT
	};

	enum class TangentAxis {
		X,
		Y,
		Z,
		COUNT
	};

	enum class AnisotropicDistribution {
		ASHIKHMIN_SHIRLEY,
		BECKMANN,
		GGX,
		MULTISCATTER_GGX,
		COUNT
	};

	enum class GlassDistribution {
		BECKMANN,
		GGX,
		MULTISCATTER_GGX,
		SHARP,
		COUNT
	};

	enum class GlossyDistribution {
		ASHIKHMIN_SHIRLEY,
		BECKMANN,
		GGX,
		MULTISCATTER_GGX,
		SHARP,
		COUNT
	};

	enum class HairComponent {
		TRANSMISSION,
		REFLECTION,
		COUNT
	};

	enum class PrincipledBSDFDistribution {
		GGX,
		MULTISCATTER_GGX,
		COUNT
	};

	enum class PrincipledBSDFSubsurfaceMethod {
		BURLEY,
		RANDOM_WALK,
		COUNT
	};

	enum class PrincipledHairColoring {
		ABSORPTION_COEFFICIENT,
		MELANIN_CONCENTRATION,
		DIRECT_COLORING,
		COUNT
	};

	enum class RefractionDistribution {
		BECKMANN,
		GGX,
		SHARP,
		COUNT
	};

	enum class SubsurfaceScatterFalloff {
		BURLEY,
		CUBIC,
		GAUSSIAN,
		RANDOM_WALK,
		COUNT
	};

	enum class ToonComponent {
		DIFFUSE,
		GLOSSY,
		COUNT
	};

	enum class MaxTexmapPrecision {
		UCHAR,
		FLOAT,
		COUNT
	};

	enum class GradientTexType {
		LINEAR,
		QUADRATIC,
		EASING,
		DIAGONAL,
		RADIAL,
		QUADRATIC_SPHERE,
		SPHERICAL,
		COUNT
	};


    enum class ImageTexInterpolationType{
        SHD_INTERP_LINEAR,
        SHD_INTERP_CLOSEST,
        SHD_INTERP_CUBIC,
        SHD_INTERP_SMART,
        COUNT
    };

    enum class ImageTexProjectionType{
        SHD_PROJ_FLAT,
        SHD_PROJ_BOX,
        SHD_PROJ_SPHERE,
        SHD_PROJ_TUBE,
        COUNT
    };

	enum class MusgraveTexDimensions {
		ONE,
		TWO,
		THREE,
		FOUR,
		COUNT
	};

	enum class MusgraveTexType {
		MULTIFRACTAL,
		RIDGED_MULTIFRACTAL,
		HYBRID_MULTIFRACTAL,
		FBM,
		HETERO_TERRAIN,
		COUNT
	};

	enum class NoiseTexDimensions {
		ONE,
		TWO,
		THREE,
		FOUR,
		COUNT
	};

	enum class VoronoiTexDimensions {
		ONE,
		TWO,
		THREE,
		FOUR,
		COUNT
	};

	enum class VoronoiTexFeature {
		F1,
		F2,
		SMOOTH_F1,
		DISTANCE_TO_EDGE,
		N_SPHERE_RADIUS,
		COUNT
	};

	enum class VoronoiTexMetric {
		EUCLIDEAN,
		MANHATTAN,
		CHEBYCHEV,
		MINKOWSKI,
		COUNT
	};

	enum class WaveTexType {
		BANDS,
		RINGS,
		COUNT
	};

	enum class WaveTexDirection {
		X,
		Y,
		Z,
		DIAGONAL,
		COUNT
	};

	enum class WaveTexProfile {
		SINE,
		SAW,
		TRIANGLE,
		COUNT
	};

	enum class WhiteNoiseTexDimensions {
		ONE,
		TWO,
		THREE,
		FOUR,
		COUNT
	};

	enum class DisplacementSpace {
		OBJECT,
		WORLD,
		COUNT
	};

	enum class VectorMappingType {
		POINT,
		TEXTURE,
		VECTOR,
		NORMAL,
		COUNT
	};

	enum class NormalMapSpace {
		TANGENT,
		OBJECT,
		WORLD,
		COUNT
	};

	enum class VectorDisplacementSpace {
		TANGENT,
		OBJECT,
		WORLD,
		COUNT
	};

	enum class VectorTransformType {
		POINT,
		VECTOR,
		NORMAL,
		COUNT
	};

	enum class VectorTransformSpace {
		CAMERA,
		OBJECT,
		WORLD,
		COUNT
	};

	class NodeEnumInfo {
	public:
		static boost::optional<NodeEnumInfo> from(NodeMetaEnum meta_enum);
		static NodeEnumInfo from(MixRGBType) { return from_assert(NodeMetaEnum::MIX_RGB_TYPE); }
		static NodeEnumInfo from(ClampType) { return from_assert(NodeMetaEnum::CLAMP_TYPE); }
		static NodeEnumInfo from(MapRangeType) { return from_assert(NodeMetaEnum::MAP_RANGE_TYPE); }
		static NodeEnumInfo from(MathType) { return from_assert(NodeMetaEnum::MATH_TYPE); }
		static NodeEnumInfo from(VectorMathType) { return from_assert(NodeMetaEnum::VECTOR_MATH_TYPE); }
		static NodeEnumInfo from(TangentDirection) { return from_assert(NodeMetaEnum::TANGENT_DIRECTION); }
		static NodeEnumInfo from(TangentAxis) { return from_assert(NodeMetaEnum::TANGENT_AXIS); }
		static NodeEnumInfo from(AnisotropicDistribution) { return from_assert(NodeMetaEnum::ANISOTROPIC_DISTRIBUTION); }
		static NodeEnumInfo from(GlassDistribution) { return from_assert(NodeMetaEnum::GLASS_DISTRIBUTION); }
		static NodeEnumInfo from(GlossyDistribution) { return from_assert(NodeMetaEnum::GLOSSY_DISTRIBUTION); }
		static NodeEnumInfo from(HairComponent) { return from_assert(NodeMetaEnum::HAIR_COMPONENT); }
		static NodeEnumInfo from(PrincipledBSDFDistribution) { return from_assert(NodeMetaEnum::PRINCIPLED_BSDF_DISTRIBUTION); }
		static NodeEnumInfo from(PrincipledBSDFSubsurfaceMethod) { return from_assert(NodeMetaEnum::PRINCIPLED_BSDF_SSS); }
		static NodeEnumInfo from(PrincipledHairColoring) { return from_assert(NodeMetaEnum::PRINCIPLED_HAIR_COLORING); }
		static NodeEnumInfo from(RefractionDistribution) { return from_assert(NodeMetaEnum::REFRACTION_DISTRIBUTION); }
		static NodeEnumInfo from(SubsurfaceScatterFalloff) { return from_assert(NodeMetaEnum::SSS_FALLOFF); }
		static NodeEnumInfo from(ToonComponent) { return from_assert(NodeMetaEnum::TOON_COMPONENT); }
		static NodeEnumInfo from(MaxTexmapPrecision) { return from_assert(NodeMetaEnum::MAX_TEXMAP_PRECISION); }
		static NodeEnumInfo from(GradientTexType) { return from_assert(NodeMetaEnum::GRADIENT_TEX_TYPE); }
        static NodeEnumInfo from(ImageTexInterpolationType) { return from_assert(NodeMetaEnum::IMAGE_TEX_InterpolationType_TYPE); }
        static NodeEnumInfo from(ImageTexProjectionType) { return from_assert(NodeMetaEnum::IMAGE_TEX_Projection_TYPE); }
		static NodeEnumInfo from(MusgraveTexDimensions) { return from_assert(NodeMetaEnum::MUSGRAVE_TEX_DIMENSIONS); }
		static NodeEnumInfo from(MusgraveTexType) { return from_assert(NodeMetaEnum::MUSGRAVE_TEX_TYPE); }
		static NodeEnumInfo from(NoiseTexDimensions) { return from_assert(NodeMetaEnum::NOISE_TEX_DIMENSIONS); }
		static NodeEnumInfo from(VoronoiTexDimensions) { return from_assert(NodeMetaEnum::VORONOI_TEX_DIMENSIONS); }
		static NodeEnumInfo from(VoronoiTexFeature) { return from_assert(NodeMetaEnum::VORONOI_TEX_FEATURE); }
		static NodeEnumInfo from(VoronoiTexMetric) { return from_assert(NodeMetaEnum::VORONOI_TEX_METRIC); }
		static NodeEnumInfo from(WaveTexType) { return from_assert(NodeMetaEnum::WAVE_TEX_TYPE); }
		static NodeEnumInfo from(WaveTexDirection) { return from_assert(NodeMetaEnum::WAVE_TEX_DIRECTION); }
		static NodeEnumInfo from(WaveTexProfile) { return from_assert(NodeMetaEnum::WAVE_TEX_PROFILE); }
		static NodeEnumInfo from(WhiteNoiseTexDimensions) { return from_assert(NodeMetaEnum::WHITE_NOISE_TEX_DIMENSIONS); }
		static NodeEnumInfo from(DisplacementSpace) { return from_assert(NodeMetaEnum::DISPLACEMENT_SPACE); }
		static NodeEnumInfo from(VectorMappingType) { return from_assert(NodeMetaEnum::VECTOR_MAPPING_TYPE); }
		static NodeEnumInfo from(NormalMapSpace) { return from_assert(NodeMetaEnum::NORMAL_MAP_SPACE); }
		static NodeEnumInfo from(VectorDisplacementSpace) { return from_assert(NodeMetaEnum::VECTOR_DISPLACEMENT_SPACE); }
		static NodeEnumInfo from(VectorTransformType) { return from_assert(NodeMetaEnum::VECTOR_TRANSFORM_TYPE); }
		static NodeEnumInfo from(VectorTransformSpace) { return from_assert(NodeMetaEnum::VECTOR_TRANSFORM_SPACE); }

		NodeMetaEnum meta_enum() const { return _meta_enum; }
		size_t count() const { return _count; }

	private:
		static NodeEnumInfo from_assert(NodeMetaEnum meta_enum);
		NodeEnumInfo(NodeMetaEnum meta_enum, size_t count) : _meta_enum{ meta_enum }, _count{ count } {}

		NodeMetaEnum _meta_enum;
		size_t _count;
	};

	class NodeEnumOptionInfo {
	public:
		static boost::optional<NodeEnumOptionInfo> from(NodeMetaEnum meta_enum, size_t option);

		const char* display_name() const;
		const char* internal_name() const;
		const char* alt_name() const;

	private:
		NodeEnumOptionInfo(NodeMetaEnum meta_enum, size_t option) : _meta_enum{ meta_enum }, _option{ option } {}

		NodeMetaEnum _meta_enum;
		size_t _option;
	};
}
