#pragma once

#include <boost/optional.hpp>

#include "../shader_core/util_enum.h"

namespace csg {

	enum class NodeCategory {
		OUTPUT,
		COLOR,
		CONVERTER,
		INPUT,
		SHADER,
		TEXTURE,
		VECTOR,
		COUNT
	};

	enum class NodeType {
		// Output
		MATERIAL_OUTPUT,
		// Color
		BRIGHTNESS_CONTRAST,
		GAMMA,
		HSV,
		INVERT,
		LIGHT_FALLOFF,
		MIX_RGB,
		RGB_CURVES,
		// Converter
		BLACKBODY,
		CLAMP,
		COLOR_RAMP,
		COMBINE_HSV,
		COMBINE_RGB,
		COMBINE_XYZ,
		MAP_RANGE,
		MATH,
		RGB_TO_BW,
		SEPARATE_HSV,
		SEPARATE_RGB,
		SEPARATE_XYZ,
		VECTOR_MATH,
		WAVELENGTH,
		// Input
		AMBIENT_OCCLUSION,
		BEVEL,
		CAMERA_DATA,
		FRESNEL,
		GEOMETRY,
		LAYER_WEIGHT,
		LIGHT_PATH,
		OBJECT_INFO,
		RGB,
		TANGENT,
		TEXTURE_COORDINATE,
		VALUE,
		WIREFRAME,
		// Shader
		ADD_SHADER,
		ANISOTROPIC_BSDF,
		DIFFUSE_BSDF,
		EMISSION,
		GLASS_BSDF,
		GLOSSY_BSDF,
        PBR,
		HAIR_BSDF,
		HOLDOUT,
		MIX_SHADER,
		PRINCIPLED_BSDF,
		PRINCIPLED_HAIR,
		PRINCIPLED_VOLUME,
		REFRACTION_BSDF,
		SUBSURFACE_SCATTER,
		TOON_BSDF,
		TRANSLUCENT_BSDF,
		TRANSPARENT_BSDF,
		VELVET_BSDF,
		VOL_ABSORPTION,
		VOL_SCATTER,
		// Texture
		MAX_TEXMAP,
		BRICK_TEX,
		CHECKER_TEX,
		GRADIENT_TEX,
        IMAGE_TEX,
		MAGIC_TEX,
		MUSGRAVE_TEX,
		NOISE_TEX,
		VORONOI_TEX,
		WAVE_TEX,
		WHITE_NOISE_TEX,
		// Vector
		BUMP,
		DISPLACEMENT,
		MAPPING,
		NORMAL,
		NORMAL_MAP,
		VECTOR_CURVES,
		VECTOR_DISPLACEMENT,
		VECTOR_TRANSFORM,
		// End
		COUNT
	};

	class NodeCategoryInfo {
	public:
		static boost::optional<NodeCategoryInfo> from(NodeCategory category);

		inline NodeCategory category() const { return _category; }
		inline const char* name() const { return _name; }
		inline bool allow_creation() const { return _allow_creation; }

	private:
		NodeCategoryInfo(NodeCategory category, const char* name, bool allow_creation = true);

		NodeCategory _category;
		const char* _name;
		bool _allow_creation;
	};

	class NodeTypeInfo {
	public:
		static boost::optional<NodeTypeInfo> from(NodeType type);
		static boost::optional<NodeTypeInfo> from(const char* type_name);

		inline NodeType type() const { return _type; }
		inline NodeCategory category() const { return _category; }
		inline const char* disp_name() const { return _disp_name; }
		inline const char* name() const { return _name; }
		inline bool allow_creation() const { return _allow_creation; }

	private:
		NodeTypeInfo(NodeType type, NodeCategory category, const char* disp_name, const char* name, bool allow_creation = true);

		NodeType _type;
		NodeCategory _category;
		const char* _disp_name;
		const char* _name;
		bool _allow_creation;
	};
	
	typedef csc::EnumList<NodeCategory, NodeCategory::COUNT> NodeCategoryList;
	typedef csc::EnumList<NodeType, NodeType::COUNT> NodeTypeList;

}
