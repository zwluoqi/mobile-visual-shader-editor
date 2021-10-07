#include "node_type.h"

#include <atomic>
#include <cassert>
#include <map>
#include <mutex>
#include <string>

boost::optional<csg::NodeCategoryInfo> csg::NodeCategoryInfo::from(const NodeCategory category)
{
	switch (category) {
		case NodeCategory::OUTPUT:
			return NodeCategoryInfo{ category, "Output", false };
		case NodeCategory::COLOR:
			return NodeCategoryInfo{ category, "Color" };
		case NodeCategory::CONVERTER:
			return NodeCategoryInfo{ category, "Converter" };
		case NodeCategory::INPUT:
			return NodeCategoryInfo{ category, "Input" };
		case NodeCategory::SHADER:
			return NodeCategoryInfo{ category, "Shader" };
		case NodeCategory::TEXTURE:
			return NodeCategoryInfo{ category, "Texture" };
		case NodeCategory::VECTOR:
			return NodeCategoryInfo{ category, "Vector" };
		default:
			return boost::none;
	}
}

csg::NodeCategoryInfo::NodeCategoryInfo(const NodeCategory category, const char* const name, const bool allow_creation) :
	_category{ category }, _name{ name }, _allow_creation{ allow_creation }
{

}

boost::optional<csg::NodeTypeInfo> csg::NodeTypeInfo::from(const NodeType type)
{
	switch (type) {
		// Output
		case NodeType::MATERIAL_OUTPUT:
			return NodeTypeInfo{ type, NodeCategory::OUTPUT, "Material Output",    "out_material",    false };
		// Color
		case NodeType::BRIGHTNESS_CONTRAST:
			return NodeTypeInfo{ type, NodeCategory::COLOR, "Brightness/Contrast",    "bright_contrast" };
		case NodeType::GAMMA:
			return NodeTypeInfo{ type, NodeCategory::COLOR, "Gamma",                  "gamma" };
		case NodeType::HSV:
			return NodeTypeInfo{ type, NodeCategory::COLOR, "HSV",                    "hsv" };
		case NodeType::INVERT:
			return NodeTypeInfo{ type, NodeCategory::COLOR, "Invert",                 "invert" };
		case NodeType::LIGHT_FALLOFF:
			return NodeTypeInfo{ type, NodeCategory::COLOR, "Light Falloff",          "light_falloff" };
		case NodeType::MIX_RGB:
			return NodeTypeInfo{ type, NodeCategory::COLOR, "Mix RGB",                "mix_rgb" };
		case NodeType::RGB_CURVES:
			return NodeTypeInfo{ type, NodeCategory::COLOR, "RGB Curves",             "rgb_curves" };
		// Converter
		case NodeType::BLACKBODY:
			return NodeTypeInfo{ type, NodeCategory::CONVERTER, "Blackbody",          "blackbody" };
		case NodeType::CLAMP:
			return NodeTypeInfo{ type, NodeCategory::CONVERTER, "Clamp",              "clamp" };
		case NodeType::COLOR_RAMP:
			return NodeTypeInfo{ type, NodeCategory::CONVERTER, "Color Ramp",         "color_ramp" };
		case NodeType::COMBINE_HSV:
			return NodeTypeInfo{ type, NodeCategory::CONVERTER, "Combine HSV",        "combine_hsv" };
		case NodeType::COMBINE_RGB:
			return NodeTypeInfo{ type, NodeCategory::CONVERTER, "Combine RGB",        "combine_rgb" };
		case NodeType::COMBINE_XYZ:
			return NodeTypeInfo{ type, NodeCategory::CONVERTER, "Combine XYZ",        "combine_xyz" };
		case NodeType::MAP_RANGE:
			return NodeTypeInfo{ type, NodeCategory::CONVERTER, "Map Range",          "map_range" };
		case NodeType::MATH:
			return NodeTypeInfo{ type, NodeCategory::CONVERTER, "Math",               "math" };
		case NodeType::RGB_TO_BW:
			return NodeTypeInfo{ type, NodeCategory::CONVERTER, "RGB to BW",          "rgb_to_bw" };
		case NodeType::SEPARATE_HSV:
			return NodeTypeInfo{ type, NodeCategory::CONVERTER, "Separate HSV",       "separate_hsv" };
		case NodeType::SEPARATE_RGB:
			return NodeTypeInfo{ type, NodeCategory::CONVERTER, "Separate RGB",       "separate_rgb" };
		case NodeType::SEPARATE_XYZ:
			return NodeTypeInfo{ type, NodeCategory::CONVERTER, "Separate XYZ",       "separate_xyz" };
		case NodeType::VECTOR_MATH:
			return NodeTypeInfo{ type, NodeCategory::CONVERTER, "Vector Math",        "vector_math" };
		case NodeType::WAVELENGTH:
			return NodeTypeInfo{ type, NodeCategory::CONVERTER, "Wavelength",         "wavelength" };
		// Input
		case NodeType::AMBIENT_OCCLUSION:
			return NodeTypeInfo{ type, NodeCategory::INPUT, "Ambient Occlusion",      "ambient_occlusion" };
		case NodeType::BEVEL:
			return NodeTypeInfo{ type, NodeCategory::INPUT, "Bevel",                  "bevel" };
		case NodeType::CAMERA_DATA:
			return NodeTypeInfo{ type, NodeCategory::INPUT, "Camera Data",            "camera_data" };
		case NodeType::FRESNEL:
			return NodeTypeInfo{ type, NodeCategory::INPUT, "Fresnel",                "fresnel" };
		case NodeType::GEOMETRY:
			return NodeTypeInfo{ type, NodeCategory::INPUT, "Geometry",               "geometry" };
		case NodeType::LAYER_WEIGHT:
			return NodeTypeInfo{ type, NodeCategory::INPUT, "Layer Weight",           "layer_weight" };
		case NodeType::LIGHT_PATH:
			return NodeTypeInfo{ type, NodeCategory::INPUT, "Light Path",             "light_path" };
		case NodeType::OBJECT_INFO:
			return NodeTypeInfo{ type, NodeCategory::INPUT, "Object Info",            "object_info" };
		case NodeType::RGB:
			return NodeTypeInfo{ type, NodeCategory::INPUT, "RGB",                    "rgb" };
		case NodeType::TANGENT:
			return NodeTypeInfo{ type, NodeCategory::INPUT, "Tangent",                "tangent" };
		case NodeType::TEXTURE_COORDINATE:
			return NodeTypeInfo{ type, NodeCategory::INPUT, "Texture Coordinate",     "texture_coordinate" };
		case NodeType::VALUE:
			return NodeTypeInfo{ type, NodeCategory::INPUT, "Value",                  "value" };
		case NodeType::WIREFRAME:
			return NodeTypeInfo{ type, NodeCategory::INPUT, "Wireframe",              "wireframe" };
		// Shader
		case NodeType::ADD_SHADER:
			return NodeTypeInfo{ type, NodeCategory::SHADER, "Add Shader",            "add_shader" };
		case NodeType::ANISOTROPIC_BSDF:
			return NodeTypeInfo{ type, NodeCategory::SHADER, "Anisotropic BSDF",      "anisotropic_bsdf" };
		case NodeType::DIFFUSE_BSDF:
			return NodeTypeInfo{ type, NodeCategory::SHADER, "Diffuse BSDF",          "diffuse_bsdf" };
		case NodeType::EMISSION:
			return NodeTypeInfo{ type, NodeCategory::SHADER, "Emission",              "emission" };
		case NodeType::GLASS_BSDF:
			return NodeTypeInfo{ type, NodeCategory::SHADER, "Glass BSDF",            "glass_bsdf" };
		case NodeType::GLOSSY_BSDF:
			return NodeTypeInfo{ type, NodeCategory::SHADER, "Glossy BSDF",           "glossy_bsdf" };
            case NodeType::PBR:
            return NodeTypeInfo{ type, NodeCategory::SHADER, "PBR",           "pbr" };
		case NodeType::HAIR_BSDF:
			return NodeTypeInfo{ type, NodeCategory::SHADER, "Hair BSDF",             "hair_bsdf" };
		case NodeType::HOLDOUT:
			return NodeTypeInfo{ type, NodeCategory::SHADER, "Holdout",               "holdout" };
		case NodeType::MIX_SHADER:
			return NodeTypeInfo{ type, NodeCategory::SHADER, "Mix Shader",            "mix_shader" };
		case NodeType::PRINCIPLED_BSDF:
			return NodeTypeInfo{ type, NodeCategory::SHADER, "Principled BSDF",       "principled_bsdf" };
		case NodeType::PRINCIPLED_HAIR:
			return NodeTypeInfo{ type, NodeCategory::SHADER, "Principled Hair",       "principled_hair" };
		case NodeType::PRINCIPLED_VOLUME:
			return NodeTypeInfo{ type, NodeCategory::SHADER, "Principled Volume",     "principled_volume" };
		case NodeType::REFRACTION_BSDF:
			return NodeTypeInfo{ type, NodeCategory::SHADER, "Refraction BSDF",       "refraction_bsdf" };
		case NodeType::SUBSURFACE_SCATTER:
			return NodeTypeInfo{ type, NodeCategory::SHADER, "Subsurface Scattering", "subsurface_scatter" };
		case NodeType::TOON_BSDF:
			return NodeTypeInfo{ type, NodeCategory::SHADER, "Toon BSDF",             "toon_bsdf"};
		case NodeType::TRANSLUCENT_BSDF:
			return NodeTypeInfo{ type, NodeCategory::SHADER, "Translucent BSDF",      "translucent_bsdf" };
		case NodeType::TRANSPARENT_BSDF:
			return NodeTypeInfo{ type, NodeCategory::SHADER, "Transparent BSDF",      "transparent_bsdf" };
		case NodeType::VELVET_BSDF:
			return NodeTypeInfo{ type, NodeCategory::SHADER, "Velvet BSDF",           "velvet_bsdf" };
		case NodeType::VOL_ABSORPTION:
			return NodeTypeInfo{ type, NodeCategory::SHADER, "Volume Absorption",     "vol_absorb" };
		case NodeType::VOL_SCATTER:
			return NodeTypeInfo{ type, NodeCategory::SHADER, "Volume Scatter",        "vol_scatter" };
		// Texture
		case NodeType::MAX_TEXMAP:
			return NodeTypeInfo{ type, NodeCategory::TEXTURE, "3ds Max Texmap",       "max_tex" };
		case NodeType::BRICK_TEX:
			return NodeTypeInfo{ type, NodeCategory::TEXTURE, "Brick Texture",        "brick_tex" };
		case NodeType::CHECKER_TEX:
			return NodeTypeInfo{ type, NodeCategory::TEXTURE, "Checker Texture",      "checker_tex" };
		case NodeType::GRADIENT_TEX:
			return NodeTypeInfo{ type, NodeCategory::TEXTURE, "Gradient Texture",     "gradient_tex" };
        case NodeType::IMAGE_TEX:
            return NodeTypeInfo{ type, NodeCategory::TEXTURE, "Image Texture",     "image_tex" };
		case NodeType::MAGIC_TEX:
			return NodeTypeInfo{ type, NodeCategory::TEXTURE, "Magic Texture",        "magic_tex" };
		case NodeType::MUSGRAVE_TEX:
			return NodeTypeInfo{ type, NodeCategory::TEXTURE, "Musgrave Texture",     "musgrave_tex" };
		case NodeType::NOISE_TEX:
			return NodeTypeInfo{ type, NodeCategory::TEXTURE, "Noise Texture",        "noise_tex" };
		case NodeType::VORONOI_TEX:
			return NodeTypeInfo{ type, NodeCategory::TEXTURE, "Voronoi Texture",      "voronoi_tex" };
		case NodeType::WAVE_TEX:
			return NodeTypeInfo{ type, NodeCategory::TEXTURE, "Wave Texture",         "wave_tex" };
		case NodeType::WHITE_NOISE_TEX:
			return NodeTypeInfo{ type, NodeCategory::TEXTURE, "White Noise Texture",  "white_noise_tex" };
		// Vector
		case NodeType::BUMP:
			return NodeTypeInfo{ type, NodeCategory::VECTOR, "Bump",                  "bump" };
		case NodeType::DISPLACEMENT:
			return NodeTypeInfo{ type, NodeCategory::VECTOR, "Displacement",          "displacement" };
		case NodeType::MAPPING:
			return NodeTypeInfo{ type, NodeCategory::VECTOR, "Mapping",               "mapping" };
		case NodeType::NORMAL:
			return NodeTypeInfo{ type, NodeCategory::VECTOR, "Normal",                "normal" };
		case NodeType::NORMAL_MAP:
			return NodeTypeInfo{ type, NodeCategory::VECTOR, "Normal Map",            "normal_map" };
		case NodeType::VECTOR_CURVES:
			return NodeTypeInfo{ type, NodeCategory::VECTOR, "Vector Curves",         "vector_curves" };
		case NodeType::VECTOR_DISPLACEMENT:
			return NodeTypeInfo{ type, NodeCategory::VECTOR, "Vector Displacement",   "vector_displacement" };
		case NodeType::VECTOR_TRANSFORM:
			return NodeTypeInfo{ type, NodeCategory::VECTOR, "Vector Transform",      "vector_transform" };
		default:
			return boost::none;
	}
}

boost::optional<csg::NodeTypeInfo> csg::NodeTypeInfo::from(const char* type_name)
{
	static std::mutex local_mutex;
	static std::atomic<bool> initialized{ false };
	static std::map<std::string, NodeType> node_type_map;

	if (initialized.load() == false) {
		std::lock_guard<std::mutex> lock{ local_mutex };
		node_type_map.clear();
		for (const NodeType this_type : NodeTypeList{}) {
			const boost::optional<NodeTypeInfo> opt_type_info = NodeTypeInfo::from(this_type);
			assert(opt_type_info.has_value());
			node_type_map[opt_type_info->name()] = opt_type_info->type();
		}
		initialized.store(true);
	}

	// Only use the const reference after the locking portion
	const std::map<std::string, NodeType>& const_map{ node_type_map };

	if (const_map.count(type_name) == 1) {
		const boost::optional<NodeTypeInfo> opt_type_info = NodeTypeInfo::from(const_map.at(type_name));
		return opt_type_info;
	}
	else {
		return boost::none;
	}
}

csg::NodeTypeInfo::NodeTypeInfo(const NodeType type, const NodeCategory category, const char* const disp_name, const char* const name, const bool allow_creation) :
	_type{ type }, _category{ category }, _disp_name{ disp_name }, _name{ name }, _allow_creation{ allow_creation }
{

}
