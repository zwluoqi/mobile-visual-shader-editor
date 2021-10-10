#include "node.h"

#include <algorithm>
#include <cfloat>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <mutex>
#include <random>

#include "node_enums.h"

static std::mutex node_id_rng_mutex;
static std::mt19937 node_id_rng;

static const float MY_PI{ static_cast<float>(acos(-1.0)) };
#define HALF_FLT_MAX (FLT_MAX*0.5)

csg::Node::Node(const NodeType type, const csc::Int2 position) : position{ position }, _type{ type }
{
	switch (type) {
		//////
		// Output
		//////
	case NodeType::MATERIAL_OUTPUT:
		_slots.push_back(Slot{ "Surface",      "surface",      SlotDirection::INPUT, SlotType::CLOSURE });
		_slots.push_back(Slot{ "Volume",       "volume",       SlotDirection::INPUT, SlotType::CLOSURE });
		_slots.push_back(Slot{ "Displacement", "displacement", SlotDirection::INPUT, SlotType::VECTOR });
		break;
		//////
		// Color
		//////
	case NodeType::BRIGHTNESS_CONTRAST:
		_slots.push_back(Slot{ "Color",    "color",    SlotDirection::OUTPUT, SlotType::COLOR });
		_slots.push_back(Slot{ "Color",    "color",    ColorSlotValue{ csc::Float3{ 1.0f, 1.0f, 1.0f} } });
		_slots.push_back(Slot{ "Bright",   "bright",   FloatSlotValue{ 0.0f, -100.0f, 100.0f } });
		_slots.push_back(Slot{ "Contrast", "contrast", FloatSlotValue{ 0.0f, -100.0f, 100.0f } });
		break;
	case NodeType::GAMMA:
		_slots.push_back(Slot{ "Color", "color", SlotDirection::OUTPUT, SlotType::COLOR });
		_slots.push_back(Slot{ "Color", "color", ColorSlotValue{ csc::Float3{ 1.0f, 1.0f, 1.0f} } });
		_slots.push_back(Slot{ "Gamma", "gamma", FloatSlotValue{ 1.0f, 0.01f, 10.0f } });
		break;
	case NodeType::HSV:
		_slots.push_back(Slot{ "Color",      "color",      SlotDirection::OUTPUT, SlotType::COLOR });
		_slots.push_back(Slot{ "Hue",        "hue",        FloatSlotValue{ 0.5f, 0.0f, 1.0f } });
		_slots.push_back(Slot{ "Saturation", "saturation", FloatSlotValue{ 1.0f, 0.0f, 2.0f } });
		_slots.push_back(Slot{ "Value",      "value",      FloatSlotValue{ 1.0f, 0.0f, 2.0f } });
		_slots.push_back(Slot{ "Fac",        "fac",        FloatSlotValue{ 1.0f, 0.0f, 1.0f } });
		_slots.push_back(Slot{ "Color",      "color",      ColorSlotValue{ csc::Float3{ 1.0f, 1.0f, 1.0f} } });
		break;
	case NodeType::INVERT:
		_slots.push_back(Slot{ "Color", "color", SlotDirection::OUTPUT, SlotType::COLOR });
		_slots.push_back(Slot{ "Fac",   "fac",   FloatSlotValue{ 1.0f, 0.0f, 1.0f } });
		_slots.push_back(Slot{ "Color", "color", ColorSlotValue{ csc::Float3{ 1.0f, 1.0f, 1.0f} } });
		break;
	case NodeType::LIGHT_FALLOFF:
		_slots.push_back(Slot{ "Quadratic", "quadratic", SlotDirection::OUTPUT, SlotType::FLOAT });
		_slots.push_back(Slot{ "Linear",    "linear",    SlotDirection::OUTPUT, SlotType::FLOAT });
		_slots.push_back(Slot{ "Constant",  "constant",  SlotDirection::OUTPUT, SlotType::FLOAT });
		_slots.push_back(Slot{ "Strength",  "strength",  FloatSlotValue{ 100.0f, 0.0f, HALF_FLT_MAX } });
		_slots.push_back(Slot{ "Smooth",    "smooth",    FloatSlotValue{ 0.0f, 0.0f, HALF_FLT_MAX } });
		break;
	case NodeType::MIX_RGB:
		_slots.push_back(Slot{ "Color",    "color",     SlotDirection::OUTPUT, SlotType::COLOR });
		_slots.push_back(Slot{ "Mix Type", "mix_type",  EnumSlotValue{ MixRGBType::MIX } });
		_slots.push_back(Slot{ "Clamp",    "use_clamp", BoolSlotValue{ false } });
		_slots.push_back(Slot{ "Fac",      "fac",       FloatSlotValue{ 0.5f, 0.0f, 1.0f } });
		_slots.push_back(Slot{ "Color1",   "color1",    ColorSlotValue{ csc::Float3{ 0.9f, 0.9f, 0.9f} } });
		_slots.push_back(Slot{ "Color2",   "color2",    ColorSlotValue{ csc::Float3{ 0.9f, 0.9f, 0.9f} } });
		_slot_aliases.push_back(std::make_pair("type", "mix_type"));
		break;
	case NodeType::RGB_CURVES:
		_slots.push_back(Slot{ "Color",  "color",  SlotDirection::OUTPUT, SlotType::COLOR });
		_slots.push_back(Slot{ "Curves", "curves", RGBCurveSlotValue{} });
		_slots.push_back(Slot{ "Fac",    "fac",    FloatSlotValue{ 1.0f, 0.0f, 1.0f } });
		_slots.push_back(Slot{ "Color",  "color",  ColorSlotValue{ csc::Float3{ 0.0f, 0.0f, 0.0f} } });
		break;
		//////
		// Converter
		//////
	case NodeType::BLACKBODY:
		_slots.push_back(Slot{ "Color",       "color",       SlotDirection::OUTPUT, SlotType::COLOR });
		_slots.push_back(Slot{ "Temperature", "temperature", FloatSlotValue{ 1500.0f, 800.0f, 20000.0f } });
		break;
	case NodeType::CLAMP:
		_slots.push_back(Slot{ "Result", "result",     SlotDirection::OUTPUT, SlotType::FLOAT });
		_slots.push_back(Slot{ "Type",   "clamp_type", EnumSlotValue{ ClampType::Clamp } });
		_slots.push_back(Slot{ "Value",  "value",      FloatSlotValue{ 1.0f, -HALF_FLT_MAX, HALF_FLT_MAX} });
		_slots.push_back(Slot{ "Min",    "min",        FloatSlotValue{ 0.0f, -HALF_FLT_MAX, HALF_FLT_MAX} });
		_slots.push_back(Slot{ "Max",    "max",        FloatSlotValue{ 1.0f, -HALF_FLT_MAX, HALF_FLT_MAX} });
		_slot_aliases.push_back(std::make_pair("type", "clamp_type"));
		break;
	case NodeType::COLOR_RAMP:
		_slots.push_back(Slot{ "Color",  "color",  SlotDirection::OUTPUT, SlotType::COLOR });
		_slots.push_back(Slot{ "Alpha",  "alpha",  SlotDirection::OUTPUT, SlotType::FLOAT });
		_slots.push_back(Slot{ "Ramp",   "ramp",   ColorRampSlotValue{} });
		_slots.push_back(Slot{ "Fac",    "fac",    FloatSlotValue{ 0.5f, 0.0f, 1.0f } });
		break;
	case NodeType::COMBINE_HSV:
		_slots.push_back(Slot{ "Color", "color", SlotDirection::OUTPUT, SlotType::COLOR });
		_slots.push_back(Slot{ "H",     "h",     FloatSlotValue{ 0.0f, 0.0f, 1.0f } });
		_slots.push_back(Slot{ "S",     "s",     FloatSlotValue{ 0.0f, 0.0f, 1.0f } });
		_slots.push_back(Slot{ "V",     "v",     FloatSlotValue{ 0.0f, 0.0f, 1.0f } });
		break;
	case NodeType::COMBINE_RGB:
		_slots.push_back(Slot{ "Image", "image", SlotDirection::OUTPUT, SlotType::COLOR });
		_slots.push_back(Slot{ "R",     "r",     FloatSlotValue{ 0.0f, 0.0f, 1.0f } });
		_slots.push_back(Slot{ "G",     "g",     FloatSlotValue{ 0.0f, 0.0f, 1.0f } });
		_slots.push_back(Slot{ "B",     "b",     FloatSlotValue{ 0.0f, 0.0f, 1.0f } });
		break;
	case NodeType::COMBINE_XYZ:
		_slots.push_back(Slot{ "Vector", "vector", SlotDirection::OUTPUT, SlotType::VECTOR });
		_slots.push_back(Slot{ "X",      "x",      FloatSlotValue{ 0.0f, 0.0f, 1e+20 } });
		_slots.push_back(Slot{ "Y",      "y",      FloatSlotValue{ 0.0f, 0.0f, 1e+20 } });
		_slots.push_back(Slot{ "Z",      "z",      FloatSlotValue{ 0.0f, 0.0f, 1e+20 } });
		break;
	case NodeType::MAP_RANGE:
		_slots.push_back(Slot{ "Result",     "result",     SlotDirection::OUTPUT, SlotType::FLOAT });
		_slots.push_back(Slot{ "Range Type", "range_type", EnumSlotValue{ MapRangeType::LINEAR } });
//		_slots.push_back(Slot{ "Clamp",      "clamp",      BoolSlotValue{ true } });
		_slots.push_back(Slot{ "Value",      "value",      FloatSlotValue{ 0.0f, -HALF_FLT_MAX, HALF_FLT_MAX } });
		_slots.push_back(Slot{ "From Min",   "from_min",   FloatSlotValue{ 0.0f, -HALF_FLT_MAX, HALF_FLT_MAX } });
		_slots.push_back(Slot{ "From Max",   "from_max",   FloatSlotValue{ 1.0f, -HALF_FLT_MAX, HALF_FLT_MAX } });
		_slots.push_back(Slot{ "To Min",     "to_min",     FloatSlotValue{ 0.0f, -HALF_FLT_MAX, HALF_FLT_MAX } });
		_slots.push_back(Slot{ "To Max",     "to_max",     FloatSlotValue{ 1.0f, -HALF_FLT_MAX, HALF_FLT_MAX } });
		_slots.push_back(Slot{ "Steps",      "steps",      FloatSlotValue{ 4.0f, 0.0f, HALF_FLT_MAX } });
		_slot_aliases.push_back(std::make_pair("type", "range_type"));
		break;
	case NodeType::MATH:
		_slots.push_back(Slot{ "Value",     "value",     SlotDirection::OUTPUT, SlotType::FLOAT });
		_slots.push_back(Slot{ "Math Type", "math_type", EnumSlotValue{ MathType::ADD } });
//		_slots.push_back(Slot{ "Clamp",     "use_clamp", BoolSlotValue{ false } });
		_slots.push_back(Slot{ "Value1",    "value1",    FloatSlotValue{ 0.0f, -HALF_FLT_MAX, HALF_FLT_MAX } });
		_slots.push_back(Slot{ "Value2",    "value2",    FloatSlotValue{ 0.0f, -HALF_FLT_MAX, HALF_FLT_MAX } });
		_slots.push_back(Slot{ "Value3",    "value3",    FloatSlotValue{ 0.0f, -HALF_FLT_MAX, HALF_FLT_MAX } });
		_slot_aliases.push_back(std::make_pair("type", "math_type"));
		break;
	case NodeType::RGB_TO_BW:
		_slots.push_back(Slot{ "Val",   "val",   SlotDirection::OUTPUT, SlotType::FLOAT });
		_slots.push_back(Slot{ "Color", "color", ColorSlotValue{ csc::Float3{ 0.9f, 0.9f, 0.9f} } });
		break;
	case NodeType::SEPARATE_HSV:
		_slots.push_back(Slot{ "H",     "h",     SlotDirection::OUTPUT, SlotType::FLOAT });
		_slots.push_back(Slot{ "S",     "s",     SlotDirection::OUTPUT, SlotType::FLOAT });
		_slots.push_back(Slot{ "V",     "v",     SlotDirection::OUTPUT, SlotType::FLOAT });
		_slots.push_back(Slot{ "Color", "color", ColorSlotValue{ csc::Float3{ 0.9f, 0.9f, 0.9f} } });
		break;
	case NodeType::SEPARATE_RGB:
		_slots.push_back(Slot{ "R",     "r",     SlotDirection::OUTPUT, SlotType::FLOAT });
		_slots.push_back(Slot{ "G",     "g",     SlotDirection::OUTPUT, SlotType::FLOAT });
		_slots.push_back(Slot{ "B",     "b",     SlotDirection::OUTPUT, SlotType::FLOAT });
		_slots.push_back(Slot{ "Image", "color", ColorSlotValue{ csc::Float3{ 0.9f, 0.9f, 0.9f} } });
		break;
	case NodeType::SEPARATE_XYZ:
		_slots.push_back(Slot{ "X",      "x",      SlotDirection::OUTPUT, SlotType::FLOAT });
		_slots.push_back(Slot{ "Y",      "y",      SlotDirection::OUTPUT, SlotType::FLOAT });
		_slots.push_back(Slot{ "Z",      "z",      SlotDirection::OUTPUT, SlotType::FLOAT });
		_slots.push_back(Slot{ "Vector", "vector", VectorSlotValue{
			csc::Float3{ 0.0f, 0.0f, 0.0f }, csc::Float3{ -HALF_FLT_MAX, -HALF_FLT_MAX, -HALF_FLT_MAX } , csc::Float3{ HALF_FLT_MAX, HALF_FLT_MAX, HALF_FLT_MAX }
		} });
		break;
	case NodeType::VECTOR_MATH:
		_slots.push_back(Slot{ "Vector",    "vector",    SlotDirection::OUTPUT, SlotType::VECTOR });
        _slots.push_back(Slot{ "Value",    "value",    SlotDirection::OUTPUT, SlotType::FLOAT });
		_slots.push_back(Slot{ "Math Type", "math_type", EnumSlotValue{ VectorMathType::ADD } });
		_slots.push_back(Slot{ "Vector1",   "vector1",   VectorSlotValue{
			csc::Float3{ 0.0f, 0.0f, 0.0f }, csc::Float3{ -HALF_FLT_MAX, -HALF_FLT_MAX, -HALF_FLT_MAX } , csc::Float3{ HALF_FLT_MAX, HALF_FLT_MAX, HALF_FLT_MAX }
		} });
		_slots.push_back(Slot{ "Vector2",   "vector2",   VectorSlotValue{
			csc::Float3{ 0.0f, 0.0f, 0.0f }, csc::Float3{ -HALF_FLT_MAX, -HALF_FLT_MAX, -HALF_FLT_MAX } , csc::Float3{ HALF_FLT_MAX, HALF_FLT_MAX, HALF_FLT_MAX }
		} });
		_slots.push_back(Slot{ "Vector3",   "vector3",   VectorSlotValue{
			csc::Float3{ 0.0f, 0.0f, 0.0f }, csc::Float3{ -HALF_FLT_MAX, -HALF_FLT_MAX, -HALF_FLT_MAX } , csc::Float3{ HALF_FLT_MAX, HALF_FLT_MAX, HALF_FLT_MAX }
		} });
		_slots.push_back(Slot{ "Scale",     "scale",     FloatSlotValue{ 1.0f,  -HALF_FLT_MAX, HALF_FLT_MAX } });
		_slot_aliases.push_back(std::make_pair("type", "math_type"));
		break;
	case NodeType::WAVELENGTH:
		_slots.push_back(Slot{ "Color",      "color",        SlotDirection::OUTPUT, SlotType::COLOR });
		_slots.push_back(Slot{ "Wavelength", "wavelength",   FloatSlotValue{ 500.0f,  380.0f, 780.0f } });
		break;
		//////
		// Input
		//////
	case NodeType::AMBIENT_OCCLUSION:
		_slots.push_back(Slot{ "Color",       "color",      SlotDirection::OUTPUT, SlotType::COLOR });
		_slots.push_back(Slot{ "AO",          "ao",         SlotDirection::OUTPUT, SlotType::FLOAT });
		_slots.push_back(Slot{ "Samples",     "samples",    IntSlotValue{ 16,  1, 128 } });
		_slots.push_back(Slot{ "Inside",      "inside",     BoolSlotValue{ false } });
		_slots.push_back(Slot{ "Only Local",  "only_local", BoolSlotValue{ false } });
		_slots.push_back(Slot{ "Color",       "color",      ColorSlotValue{ csc::Float3{ 1.0f, 1.0f, 1.0f} } });
		_slots.push_back(Slot{ "Distance",    "distance",   FloatSlotValue{ 1.0f,  0.0f, HALF_FLT_MAX } });
		_slots.push_back(Slot{ "Normal",      "normal",     SlotDirection::INPUT, SlotType::VECTOR ,"GPU_WORLD_NORMAL"});
		break;
	case NodeType::BEVEL:
		_slots.push_back(Slot{ "Normal",  "normal",  SlotDirection::OUTPUT, SlotType::VECTOR });
		_slots.push_back(Slot{ "Samples", "samples", IntSlotValue{ 4,  2, 16 } });
		_slots.push_back(Slot{ "Radius",  "radius",  FloatSlotValue{ 0.5f, 0.0f, HALF_FLT_MAX } });
		_slots.push_back(Slot{ "Normal",  "normal",  SlotDirection::INPUT, SlotType::VECTOR, "GPU_WORLD_NORMAL" });
		break;
	case NodeType::CAMERA_DATA:
		_slots.push_back(Slot{ "View Vector",   "view_vector",    SlotDirection::OUTPUT, SlotType::VECTOR });
		_slots.push_back(Slot{ "View Z Depth",  "view_z_depth",   SlotDirection::OUTPUT, SlotType::FLOAT });
		_slots.push_back(Slot{ "View Distance", "view_distance",  SlotDirection::OUTPUT, SlotType::FLOAT });
		break;
	case NodeType::FRESNEL:
		_slots.push_back(Slot{ "Fac",    "fac",    SlotDirection::OUTPUT, SlotType::FLOAT });
		_slots.push_back(Slot{ "IOR",    "IOR",    FloatSlotValue{ 1.45f, 0.0f, 100.0f } });
		_slots.push_back(Slot{ "Normal", "normal", SlotDirection::INPUT, SlotType::VECTOR,"GPU_WORLD_NORMAL" });
		break;
	case NodeType::GEOMETRY:
		_slots.push_back(Slot{ "Position",           "position",           SlotDirection::OUTPUT, SlotType::VECTOR });
		_slots.push_back(Slot{ "Normal",             "normal",             SlotDirection::OUTPUT, SlotType::VECTOR });
		_slots.push_back(Slot{ "Tangent",            "tangent",            SlotDirection::OUTPUT, SlotType::VECTOR });
		_slots.push_back(Slot{ "True Normal",        "true_normal",        SlotDirection::OUTPUT, SlotType::VECTOR });
		_slots.push_back(Slot{ "Incoming",           "incoming",           SlotDirection::OUTPUT, SlotType::VECTOR });
		_slots.push_back(Slot{ "Parametric",         "parametric",         SlotDirection::OUTPUT, SlotType::VECTOR });
		_slots.push_back(Slot{ "Backfacing",         "backfacing",         SlotDirection::OUTPUT, SlotType::FLOAT });
		_slots.push_back(Slot{ "Pointiness",         "pointiness",         SlotDirection::OUTPUT, SlotType::FLOAT });
		_slots.push_back(Slot{ "Random Per Island",  "random_per_island",  SlotDirection::OUTPUT, SlotType::FLOAT });
		break;
	case NodeType::LAYER_WEIGHT:
		_slots.push_back(Slot{ "Fresnel", "fresnel", SlotDirection::OUTPUT, SlotType::FLOAT });
		_slots.push_back(Slot{ "Facing",  "facing",  SlotDirection::OUTPUT, SlotType::FLOAT });
		_slots.push_back(Slot{ "Blend",   "blend",   FloatSlotValue{ 0.5f, 0.0f, 1.0f } });
		_slots.push_back(Slot{ "Normal",  "normal",  SlotDirection::INPUT, SlotType::VECTOR,"GPU_WORLD_NORMAL" });
		break;
	case NodeType::LIGHT_PATH:
		_slots.push_back(Slot{ "Is Camera Ray",       "is_camera_ray",       SlotDirection::OUTPUT, SlotType::FLOAT });
		_slots.push_back(Slot{ "Is Shadow Ray",       "is_shadow_ray",       SlotDirection::OUTPUT, SlotType::FLOAT });
		_slots.push_back(Slot{ "Is Diffuse Ray",      "is_diffuse_ray",      SlotDirection::OUTPUT, SlotType::FLOAT });
		_slots.push_back(Slot{ "Is Glossy Ray",       "is_glossy_ray",       SlotDirection::OUTPUT, SlotType::FLOAT });
		_slots.push_back(Slot{ "Is Singular Ray",     "is_singular_ray",     SlotDirection::OUTPUT, SlotType::FLOAT });
		_slots.push_back(Slot{ "Is Reflection Ray",   "is_reflection_ray",   SlotDirection::OUTPUT, SlotType::FLOAT });
		_slots.push_back(Slot{ "Is Transmission Ray", "is_transmission_ray", SlotDirection::OUTPUT, SlotType::FLOAT });
		_slots.push_back(Slot{ "Ray Length",          "ray_length",          SlotDirection::OUTPUT, SlotType::FLOAT });
		_slots.push_back(Slot{ "Ray Depth",           "ray_depth",           SlotDirection::OUTPUT, SlotType::FLOAT });
		_slots.push_back(Slot{ "Diffuse Depth",       "diffuse_depth",       SlotDirection::OUTPUT, SlotType::FLOAT });
		_slots.push_back(Slot{ "Glossy Depth",        "glossy_depth",        SlotDirection::OUTPUT, SlotType::FLOAT });
		_slots.push_back(Slot{ "Transparent Depth",   "transparent_depth",   SlotDirection::OUTPUT, SlotType::FLOAT });
		_slots.push_back(Slot{ "Transmission Depth",  "transmission_depth",  SlotDirection::OUTPUT, SlotType::FLOAT });
		break;
	case NodeType::OBJECT_INFO:
		_slots.push_back(Slot{ "Location",       "location",     SlotDirection::OUTPUT, SlotType::VECTOR });
		_slots.push_back(Slot{ "Color",          "color",        SlotDirection::OUTPUT, SlotType::COLOR });
		_slots.push_back(Slot{ "Object Index",   "object_index", SlotDirection::OUTPUT, SlotType::FLOAT });
		_slots.push_back(Slot{ "Material Index", "material",     SlotDirection::OUTPUT, SlotType::FLOAT });
		_slots.push_back(Slot{ "Random",         "random",       SlotDirection::OUTPUT, SlotType::FLOAT });
		break;
	case NodeType::RGB:
		_slots.push_back(Slot{ "Color", "color", SlotDirection::OUTPUT, SlotType::COLOR });
		_slots.push_back(Slot{ "Value", "value", ColorSlotValue{ csc::Float3{ 0.9f, 0.9f, 0.9f} } });
		break;
	case NodeType::TANGENT:
		_slots.push_back(Slot{ "Tangent",     "tangent",   SlotDirection::OUTPUT, SlotType::VECTOR });
		_slots.push_back(Slot{ "Direction",   "direction", EnumSlotValue{ TangentDirection::RADIAL } });
		_slots.push_back(Slot{ "Radial Axis", "axis",      EnumSlotValue{ TangentAxis::Z } });
		break;
	case NodeType::TEXTURE_COORDINATE:
		_slots.push_back(Slot{ "Generated",  "generated",  SlotDirection::OUTPUT, SlotType::VECTOR });
		_slots.push_back(Slot{ "Normal",     "normal",     SlotDirection::OUTPUT, SlotType::VECTOR });
		_slots.push_back(Slot{ "UV",         "UV",         SlotDirection::OUTPUT, SlotType::VECTOR });
		_slots.push_back(Slot{ "Object",     "object",     SlotDirection::OUTPUT, SlotType::VECTOR });
		_slots.push_back(Slot{ "Camera",     "camera",     SlotDirection::OUTPUT, SlotType::VECTOR });
		_slots.push_back(Slot{ "Window",     "window",     SlotDirection::OUTPUT, SlotType::VECTOR });
		_slots.push_back(Slot{ "Reflection", "reflection", SlotDirection::OUTPUT, SlotType::VECTOR });
		break;
	case NodeType::VALUE:
		_slots.push_back(Slot{ "Value", "value", SlotDirection::OUTPUT, SlotType::FLOAT });
		_slots.push_back(Slot{ "Value", "value", FloatSlotValue{ 0.0f,  -HALF_FLT_MAX, HALF_FLT_MAX } });
		break;
	case NodeType::WIREFRAME:
		_slots.push_back(Slot{ "Fac",            "fac",              SlotDirection::OUTPUT, SlotType::FLOAT });
		_slots.push_back(Slot{ "Use Pixel Size", "use_pixel_size",   BoolSlotValue{ false } });
		_slots.push_back(Slot{ "Size",           "size",             FloatSlotValue{ 0.1f,  0.0f, HALF_FLT_MAX } });
		break;
		//////
		// Shader
		//////
	case NodeType::ADD_SHADER:
		_slots.push_back(Slot{ "Closure",  "closure",  SlotDirection::OUTPUT, SlotType::CLOSURE });
		_slots.push_back(Slot{ "Closure1", "closure1", SlotDirection::INPUT,  SlotType::CLOSURE });
		_slots.push_back(Slot{ "Closure2", "closure2", SlotDirection::INPUT,  SlotType::CLOSURE });
		break;
	case NodeType::ANISOTROPIC_BSDF:
		_slots.push_back(Slot{ "BSDF",         "BSDF",         SlotDirection::OUTPUT, SlotType::CLOSURE });
		_slots.push_back(Slot{ "Distribution", "distribution", EnumSlotValue{ AnisotropicDistribution::GGX } });
		_slots.push_back(Slot{ "Color",        "color",        ColorSlotValue{ csc::Float3{ 0.9f, 0.9f, 0.9f} } });
		_slots.push_back(Slot{ "Roughness",    "roughness",    FloatSlotValue{ 0.0f,  0.0f, 1.0f } });
		_slots.push_back(Slot{ "Anisotropy",   "anisotropy",   FloatSlotValue{ 0.5f, -1.0f, 1.0f } });
		_slots.push_back(Slot{ "Rotation",     "rotation",     FloatSlotValue{ 0.0f,  0.0f, 1.0f } });
		_slots.push_back(Slot{ "Normal",       "normal",       SlotDirection::INPUT, SlotType::VECTOR });
		_slots.push_back(Slot{ "Tangent",      "tangent",      SlotDirection::INPUT, SlotType::VECTOR });
		break;
	case NodeType::DIFFUSE_BSDF:
		_slots.push_back(Slot{ "BSDF",      "BSDF",      SlotDirection::OUTPUT, SlotType::CLOSURE });
		_slots.push_back(Slot{ "Color",     "color",     ColorSlotValue{ csc::Float3{ 0.9f, 0.9f, 0.9f} } });
		_slots.push_back(Slot{ "Roughness", "roughness", FloatSlotValue{ 0.0f, 0.0f, 1.0f } });
		_slots.push_back(Slot{ "Normal",    "normal",    SlotDirection::INPUT, SlotType::VECTOR ,"GPU_WORLD_NORMAL"});
		break;
	case NodeType::EMISSION:
		_slots.push_back(Slot{ "Emission", "emission", SlotDirection::OUTPUT, SlotType::CLOSURE });
		_slots.push_back(Slot{ "Color",    "color",    ColorSlotValue{ csc::Float3{ 0.9f, 0.9f, 0.9f} } });
		_slots.push_back(Slot{ "Strength", "strength", FloatSlotValue{ 1.0f, 0.0f, HALF_FLT_MAX } });
		break;
	case NodeType::GLASS_BSDF:
		_slots.push_back(Slot{ "BSDF",         "BSDF",         SlotDirection::OUTPUT, SlotType::CLOSURE });
		_slots.push_back(Slot{ "Distribution", "distribution", EnumSlotValue{ GlassDistribution::GGX } });
		_slots.push_back(Slot{ "Color",        "color",        ColorSlotValue{ csc::Float3{ 0.9f, 0.9f, 0.9f} } });
		_slots.push_back(Slot{ "Roughness",    "roughness",    FloatSlotValue{ 0.0f, 0.0f, 1.0f } });
		_slots.push_back(Slot{ "IOR",          "IOR",          FloatSlotValue{ 1.45f, 0.0f, 100.0f } });
		_slots.push_back(Slot{ "Normal",       "normal",       SlotDirection::INPUT, SlotType::VECTOR });
		break;
	case NodeType::GLOSSY_BSDF:
		_slots.push_back(Slot{ "BSDF",         "BSDF",         SlotDirection::OUTPUT, SlotType::CLOSURE });
		_slots.push_back(Slot{ "Distribution", "distribution", EnumSlotValue{ GlossyDistribution::GGX } });
		_slots.push_back(Slot{ "Color",        "color",        ColorSlotValue{ csc::Float3{ 0.9f, 0.9f, 0.9f} } });
		_slots.push_back(Slot{ "Roughness",    "roughness",    FloatSlotValue{ 0.0f, 0.0f, 1.0f } });
		_slots.push_back(Slot{ "Normal",       "normal",       SlotDirection::INPUT, SlotType::VECTOR });
		break;
	case NodeType::HAIR_BSDF:
		_slots.push_back(Slot{ "BSDF",       "BSDF",        SlotDirection::OUTPUT, SlotType::CLOSURE });
		_slots.push_back(Slot{ "Component",  "component",   EnumSlotValue{ HairComponent::REFLECTION } });
		_slots.push_back(Slot{ "Color",      "color",       ColorSlotValue{ csc::Float3{ 0.9f, 0.9f, 0.9f} } });
		_slots.push_back(Slot{ "Offset",     "offset",      FloatSlotValue{ 0.0f, -90.0f, 90.0f, 2 } });
		_slots.push_back(Slot{ "RoughnessU", "roughness_u", FloatSlotValue{ 0.1f, 0.0f, 1.0f } });
		_slots.push_back(Slot{ "RoughnessV", "roughness_v", FloatSlotValue{ 1.0f, 0.0f, 1.0f } });
		_slots.push_back(Slot{ "Tangent",    "tangent",     SlotDirection::INPUT, SlotType::VECTOR });
		break;
    case NodeType::PBR:
        _slots.push_back(Slot{ "BSDF",         "BSDF",         SlotDirection::OUTPUT, SlotType::CLOSURE });
        _slots.push_back(Slot{ "DiffuseResult",         "diffuse",         SlotDirection::OUTPUT, SlotType::COLOR });
        _slots.push_back(Slot{ "SpeResult",         "specular",         SlotDirection::OUTPUT, SlotType::COLOR });
        _slots.push_back(Slot{ "SurfaceReduction",         "surface_reduction",         SlotDirection::OUTPUT, SlotType::COLOR });
        _slots.push_back(Slot{ "Albedo",      "albedo",       ColorSlotValue{ csc::Float3{ 0.9f, 0.9f, 0.9f} } });
        _slots.push_back(Slot{ "Specular",      "specular",       ColorSlotValue{ csc::Float3{ 0.0f, 0.0f, 0.0f} } });
        _slots.push_back(Slot{ "AlbedoOcclusion",      "occlusion",       FloatSlotValue{ 0.0f, 0.0f, 1.0f } } );
        _slots.push_back(Slot{ "Emission",      "emission",       ColorSlotValue{ csc::Float3{ 0.0f, 0.0f, 0.0f} } });
        _slots.push_back(Slot{ "Smoothness",    "smoothness",    FloatSlotValue{ 0.0f, 0.0f, 1.0f } });
        _slots.push_back(Slot{ "Normal",       "normal",       SlotDirection::INPUT, SlotType::VECTOR ,"GPU_WORLD_NORMAL"});
        break;
	case NodeType::HOLDOUT:
		_slots.push_back(Slot{ "Holdout", "holdout", SlotDirection::OUTPUT, SlotType::CLOSURE });
		break;
	case NodeType::MIX_SHADER:
		_slots.push_back(Slot{ "Closure",  "closure",  SlotDirection::OUTPUT, SlotType::CLOSURE });
		_slots.push_back(Slot{ "Fac",      "fac",      FloatSlotValue{ 0.5f, 0.0f, 1.0f } });
		_slots.push_back(Slot{ "Closure1", "closure1", SlotDirection::INPUT, SlotType::CLOSURE });
		_slots.push_back(Slot{ "Closure2", "closure2", SlotDirection::INPUT, SlotType::CLOSURE });
		break;
	case NodeType::PRINCIPLED_BSDF:
		_slots.push_back(Slot{ "BSDF",                "BSDF",                 SlotDirection::OUTPUT, SlotType::CLOSURE });
		_slots.push_back(Slot{ "Distribution",        "distribution",         EnumSlotValue{ PrincipledBSDFDistribution::GGX } });
		_slots.push_back(Slot{ "Base Color",          "base_color",           ColorSlotValue{ csc::Float3{ 0.8f, 0.8f, 0.8f} } });
		_slots.push_back(Slot{ "Subsurface Method",   "subsurface_method",    EnumSlotValue{ PrincipledBSDFSubsurfaceMethod::BURLEY } });
		_slots.push_back(Slot{ "Subsurface",          "subsurface",           FloatSlotValue{ 0.0f, 0.0f, 1.0f } });
		_slots.push_back(Slot{ "Subsurface Radius",   "subsurface_radius",    VectorSlotValue{
			csc::Float3{ 1.0f, 0.2f, 0.1f }, csc::Float3{ 0.0f, 0.0f, 0.0f } , csc::Float3{ HALF_FLT_MAX, HALF_FLT_MAX, HALF_FLT_MAX }
		}});
		_slots.push_back(Slot{ "Subsurface Color",       "subsurface_color",       ColorSlotValue{ csc::Float3{ 0.7f, 1.0f, 1.0f} } });
		_slots.push_back(Slot{ "Metallic",               "metallic",               FloatSlotValue{ 0.0f, 0.0f, 1.0f } });
		_slots.push_back(Slot{ "Specular",               "specular",               FloatSlotValue{ 0.5f, 0.0f, 1.0f } });
		_slots.push_back(Slot{ "Specular Tint",          "specular_tint",          FloatSlotValue{ 0.0f, 0.0f, 1.0f } });
		_slots.push_back(Slot{ "Roughness",              "roughness",              FloatSlotValue{ 0.5f, 0.0f, 1.0f } });
		_slots.push_back(Slot{ "Anisotropic",            "anisotropic",            FloatSlotValue{ 0.0f, 0.0f, 1.0f } });
		_slots.push_back(Slot{ "Anisotropic Rotation",   "anisotropic_rotation",   FloatSlotValue{ 0.0f, 0.0f, 1.0f } });
		_slots.push_back(Slot{ "Sheen",                  "sheen",                  FloatSlotValue{ 0.0f, 0.0f, 1.0f } });
		_slots.push_back(Slot{ "Sheen Tint",             "sheen_tint",             FloatSlotValue{ 0.5f, 0.0f, 1.0f } });
		_slots.push_back(Slot{ "Clearcoat",              "clearcoat",              FloatSlotValue{ 0.0f, 0.0f, 1.0f } });
		_slots.push_back(Slot{ "Clearcoat Roughness",    "clearcoat_roughness",    FloatSlotValue{ 0.0f, 0.0f, 1.0f } });
		_slots.push_back(Slot{ "IOR",                    "ior",                    FloatSlotValue{ 1.45f, 0.0f, 100.0f } });
		_slots.push_back(Slot{ "Transmission",           "transmission",           FloatSlotValue{ 0.0f, 0.0f, 1.0f } });
		_slots.push_back(Slot{ "Transmission Roughness", "transmission_roughness", FloatSlotValue{ 0.0f, 0.0f, 1.0f } });
		_slots.push_back(Slot{ "Emission",               "emission",               ColorSlotValue{ csc::Float3{ 0.0f, 0.0f, 0.0f } } });
		_slots.push_back(Slot{ "Emission Strength",      "emission_strength",      FloatSlotValue{ 1.0f, 0.0f, 10000.0f } });
		_slots.push_back(Slot{ "Alpha",                  "alpha",                  FloatSlotValue{ 1.0f, 0.0f, 1.0f } });
		_slots.push_back(Slot{ "Normal",                 "normal",                 SlotDirection::INPUT, SlotType::VECTOR });
		_slots.push_back(Slot{ "Clearcoat Normal",       "clearcoat_normal",       SlotDirection::INPUT, SlotType::VECTOR });
		_slots.push_back(Slot{ "Tangent",                "tangent",                SlotDirection::INPUT, SlotType::VECTOR });
		break;
	case NodeType::PRINCIPLED_HAIR:
		_slots.push_back(Slot{ "BSDF",                   "BSDF",                   SlotDirection::OUTPUT, SlotType::CLOSURE });
		_slots.push_back(Slot{ "Coloring",               "coloring",               EnumSlotValue{ PrincipledHairColoring::DIRECT_COLORING } });
		_slots.push_back(Slot{ "Color",                  "color",                  ColorSlotValue{ csc::Float3{ 0.017513f, 0.005763f, 0.002059f } } });
		_slots.push_back(Slot{ "Melanin",                "melanin",                FloatSlotValue{ 0.8f, 0.0f, 1.0f } });
		_slots.push_back(Slot{ "Melanin Redness",        "melanin_redness",        FloatSlotValue{ 1.0f, 0.0f, 1.0f } });
		_slots.push_back(Slot{ "Tint",                   "tint",                   ColorSlotValue{ csc::Float3{ 1.0f, 1.0f, 1.0f } } });
		_slots.push_back(Slot{ "Absorption Coefficient", "absorption_coefficient", VectorSlotValue{
			csc::Float3{ 0.245531f, 0.52f, 1.365f }, csc::Float3{ 0.0f, 0.0f, 0.0f } , csc::Float3{ HALF_FLT_MAX, HALF_FLT_MAX, HALF_FLT_MAX }
		} });
		_slots.push_back(Slot{ "Roughness",              "roughness",              FloatSlotValue{ 0.3f, 0.0f, 1.0f } });
		_slots.push_back(Slot{ "Radial Roughness",       "radial_roughness",       FloatSlotValue{ 0.3f, 0.0f, 1.0f } });
		_slots.push_back(Slot{ "Coat",                   "coat",                   FloatSlotValue{ 0.0f, 0.0f, 1.0f } });
		_slots.push_back(Slot{ "IOR",                    "ior",                    FloatSlotValue{ 1.55f, 0.0f, 1000.0f } });
		_slots.push_back(Slot{ "Offset",                 "offset",                 FloatSlotValue{ 2 * MY_PI / 180.0f, MY_PI / -2.0f , MY_PI / 2.0f } });
		_slots.push_back(Slot{ "Random Roughness",       "random_roughness",       FloatSlotValue{ 0.0f, 0.0f, 1.0f } });
		_slots.push_back(Slot{ "Random Color",           "random_color",           FloatSlotValue{ 0.0f, 0.0f, 1.0f } });
		_slots.push_back(Slot{ "Random",                 "random",                 FloatSlotValue{ 0.0f, 0.0f, HALF_FLT_MAX } });
		break;
	case NodeType::PRINCIPLED_VOLUME:
		_slots.push_back(Slot{ "Volume",              "volume",              SlotDirection::OUTPUT, SlotType::CLOSURE });
		_slots.push_back(Slot{ "Color",               "color",               ColorSlotValue{ csc::Float3{ 0.5f, 0.5f, 0.5f } } });
		_slots.push_back(Slot{ "Density",             "density",             FloatSlotValue{ 1.0f, 0.0f, HALF_FLT_MAX } });
		_slots.push_back(Slot{ "Anisotropy",          "anisotropy",          FloatSlotValue{ 0.0f, -1.0f, 1.0f } });
		_slots.push_back(Slot{ "Absorption Color",    "absorption_color",    ColorSlotValue{ csc::Float3{ 0.0f, 0.0f, 0.0f } } });
		_slots.push_back(Slot{ "Emission Strength",   "emission_strength",   FloatSlotValue{ 0.0f, 0.0f, HALF_FLT_MAX } });
		_slots.push_back(Slot{ "Emission Color",      "emission_color",      ColorSlotValue{ csc::Float3{ 1.0f, 1.0f, 1.0f } } });
		_slots.push_back(Slot{ "Blackbody Intensity", "blackbody_intensity", FloatSlotValue{ 0.0f, 0.0f, 1.0f } });
		_slots.push_back(Slot{ "Blackbody Tint",      "blackbody_tint",      ColorSlotValue{ csc::Float3{ 1.0f, 1.0f, 1.0f } } });
		_slots.push_back(Slot{ "Temperature",         "temperature",         FloatSlotValue{ 1000.0f, 0.0f, 8000.0f } });
		break;
	case NodeType::REFRACTION_BSDF:
		_slots.push_back(Slot{ "BSDF",         "BSDF",         SlotDirection::OUTPUT, SlotType::CLOSURE });
		_slots.push_back(Slot{ "Distribution", "distribution", EnumSlotValue{ RefractionDistribution::GGX } });
		_slots.push_back(Slot{ "Color",        "color",        ColorSlotValue{ csc::Float3{ 0.9f, 0.9f, 0.9f} } });
		_slots.push_back(Slot{ "Roughness",    "roughness",    FloatSlotValue{ 0.0f, 0.0f, 1.0f } });
		_slots.push_back(Slot{ "IOR",          "IOR",          FloatSlotValue{ 1.45f, 0.0f, 100.0f } });
		_slots.push_back(Slot{ "Normal",       "normal",       SlotDirection::INPUT, SlotType::VECTOR });
		break;
	case NodeType::SUBSURFACE_SCATTER:
		_slots.push_back(Slot{ "BSSRDF",       "BSSRDF",       SlotDirection::OUTPUT, SlotType::CLOSURE });
		_slots.push_back(Slot{ "Falloff",      "falloff",      EnumSlotValue{ SubsurfaceScatterFalloff::BURLEY } });
		_slots.push_back(Slot{ "Color",        "color",        ColorSlotValue{ csc::Float3{ 0.9f, 0.9f, 0.9f} } });
		_slots.push_back(Slot{ "Scale",        "scale",        FloatSlotValue{ 1.0f, 0.0f, HALF_FLT_MAX } });
		_slots.push_back(Slot{ "Radius",       "radius",       VectorSlotValue{
			csc::Float3{ 1.0f, 1.0f, 1.0f }, csc::Float3{ 0.0f, 0.0f, 0.0f } , csc::Float3{ HALF_FLT_MAX, HALF_FLT_MAX, HALF_FLT_MAX }
		} });
		_slots.push_back(Slot{ "Sharpness",    "sharpness",    FloatSlotValue{ 0.0f, 0.0f, 1.0f } });
		_slots.push_back(Slot{ "Texture Blur", "texture_blur", FloatSlotValue{ 0.0f, 0.0f, 1.0f } });
		_slots.push_back(Slot{ "Normal",       "normal",       SlotDirection::INPUT, SlotType::VECTOR });
		break;
	case NodeType::TOON_BSDF:
		_slots.push_back(Slot{ "BSDF",      "BSDF",      SlotDirection::OUTPUT, SlotType::CLOSURE });
		_slots.push_back(Slot{ "Component", "component", EnumSlotValue{ ToonComponent::DIFFUSE } });
		_slots.push_back(Slot{ "Color",     "color",     ColorSlotValue{ csc::Float3{ 0.9f, 0.9f, 0.9f} } });
		_slots.push_back(Slot{ "Size",      "size",      FloatSlotValue{ 0.5f, 0.0f, 1.0f } });
		_slots.push_back(Slot{ "Smooth",    "smooth",    FloatSlotValue{ 0.0f, 0.0f, 1.0f } });
		_slots.push_back(Slot{ "Normal",    "normal",    SlotDirection::INPUT, SlotType::VECTOR });
		break;
	case NodeType::TRANSLUCENT_BSDF:
		_slots.push_back(Slot{ "BSDF",   "BSDF",   SlotDirection::OUTPUT, SlotType::CLOSURE });
		_slots.push_back(Slot{ "Color",  "color",  ColorSlotValue{ csc::Float3{ 0.9f, 0.9f, 0.9f} } });
		_slots.push_back(Slot{ "Normal", "normal", SlotDirection::INPUT, SlotType::VECTOR });
		break;
	case NodeType::TRANSPARENT_BSDF:
		_slots.push_back(Slot{ "BSDF",  "BSDF",  SlotDirection::OUTPUT, SlotType::CLOSURE });
		_slots.push_back(Slot{ "Color", "color", ColorSlotValue{ csc::Float3{ 0.9f, 0.9f, 0.9f} } });
		break;
	case NodeType::VELVET_BSDF:
		_slots.push_back(Slot{ "BSDF",   "BSDF",   SlotDirection::OUTPUT, SlotType::CLOSURE });
		_slots.push_back(Slot{ "Color",  "color",  ColorSlotValue{ csc::Float3{ 0.9f, 0.9f, 0.9f} } });
		_slots.push_back(Slot{ "Sigma",  "sigma",  FloatSlotValue{ 1.0f, 0.0f, 1.0f } });
		_slots.push_back(Slot{ "Normal", "normal", SlotDirection::INPUT, SlotType::VECTOR });
		break;
	case NodeType::VOL_ABSORPTION:
		_slots.push_back(Slot{ "Volume",  "volume",  SlotDirection::OUTPUT, SlotType::CLOSURE });
		_slots.push_back(Slot{ "Color",   "color",   ColorSlotValue{ csc::Float3{ 0.9f, 0.9f, 0.9f} } });
		_slots.push_back(Slot{ "Density", "density", FloatSlotValue{ 1.0f, 0.0f, HALF_FLT_MAX } });
		break;
	case NodeType::VOL_SCATTER:
		_slots.push_back(Slot{ "Volume",     "volume",     SlotDirection::OUTPUT, SlotType::CLOSURE });
		_slots.push_back(Slot{ "Color",      "color",      ColorSlotValue{ csc::Float3{ 0.9f, 0.9f, 0.9f} } });
		_slots.push_back(Slot{ "Density",    "density",    FloatSlotValue{ 1.0f, 0.0f, HALF_FLT_MAX } });
		_slots.push_back(Slot{ "Anisotropy", "anisotropy", FloatSlotValue{ 0.0f, -1.0f, 1.0f } });
		break;
		//////
		// Texture
		//////
	case NodeType::BRICK_TEX:
		_slots.push_back(Slot{ "Color",            "color",               SlotDirection::OUTPUT, SlotType::COLOR });
		_slots.push_back(Slot{ "Fac",              "fac",                 SlotDirection::OUTPUT, SlotType::FLOAT });
		_slots.push_back(Slot{ "Offset",           "offset",              FloatSlotValue{ 0.0f, 0.0f, 1.0f } });
		_slots.push_back(Slot{ "Offset Frequency", "offset_frequency",    IntSlotValue{ 2, 1, 99 } });
		_slots.push_back(Slot{ "Squash",           "squash",              FloatSlotValue{ 0.0f, 0.0f, 1.0f } });
		_slots.push_back(Slot{ "Squash Frequency", "squash_frequency",    IntSlotValue{ 2, 1, 99 } });
		_slots.push_back(Slot{ "Vector",           "vector",              SlotDirection::INPUT, SlotType::VECTOR, "VERTEX_CD_ORCO" });
		_slots.push_back(Slot{ "Color1",           "color1",              ColorSlotValue{ csc::Float3{ 0.9f, 0.9f, 0.9f} } });
		_slots.push_back(Slot{ "Color2",           "color2",              ColorSlotValue{ csc::Float3{ 0.4f, 0.4f, 0.4f} } });
		_slots.push_back(Slot{ "Mortar",           "mortar",              ColorSlotValue{ csc::Float3{ 0.4f, 0.4f, 0.4f} } });
		_slots.push_back(Slot{ "Scale",            "scale",               FloatSlotValue{ 5.0f, -HALF_FLT_MAX, HALF_FLT_MAX } });
		_slots.push_back(Slot{ "Mortar Size",      "mortar_size",         FloatSlotValue{ 0.08f, 0.0f, 5.0f } });
		_slots.push_back(Slot{ "Mortar Smooth",    "mortar_smooth",       FloatSlotValue{ 0.1f, 0.0f, 10.0f } });
		_slots.push_back(Slot{ "Bias",             "bias",                FloatSlotValue{ 0.0f, -10.0f, 10.0f } });
		_slots.push_back(Slot{ "Brick Width",      "brick_width",         FloatSlotValue{ 0.5f, 0.0f, 10.0f } });
		_slots.push_back(Slot{ "Row Height",       "row_height",          FloatSlotValue{ 0.25f, 0.0f, 10.0f } });
		break;
	case NodeType::CHECKER_TEX:
		_slots.push_back(Slot{ "Color",  "color",  SlotDirection::OUTPUT, SlotType::COLOR });
		_slots.push_back(Slot{ "Fac",    "fac",    SlotDirection::OUTPUT, SlotType::FLOAT });
		_slots.push_back(Slot{ "Vector", "vector", SlotDirection::INPUT, SlotType::VECTOR,"VERTEX_CD_ORCO" });
		_slots.push_back(Slot{ "Color1", "color1", ColorSlotValue{ csc::Float3{ 0.9f, 0.9f, 0.9f} } });
		_slots.push_back(Slot{ "Color2", "color2", ColorSlotValue{ csc::Float3{ 0.4f, 0.4f, 0.4f} } });
		_slots.push_back(Slot{ "Scale",  "scale",  FloatSlotValue{ 5.0f, -HALF_FLT_MAX, HALF_FLT_MAX } });
		break;
	case NodeType::GRADIENT_TEX:
		_slots.push_back(Slot{ "Color",  "color",         SlotDirection::OUTPUT, SlotType::COLOR });
		_slots.push_back(Slot{ "Fac",    "fac",           SlotDirection::OUTPUT, SlotType::FLOAT });
		_slots.push_back(Slot{ "Type",   "gradient_type", EnumSlotValue{ GradientTexType::LINEAR } });
		_slots.push_back(Slot{ "Vector", "vector",        SlotDirection::INPUT, SlotType::VECTOR,"VERTEX_CD_ORCO" });
		_slot_aliases.push_back(std::make_pair("type", "gradient_type"));
		break;
    case NodeType::IMAGE_TEX:
        _slots.push_back(Slot{ "Color",      "color",      SlotDirection::OUTPUT, SlotType::COLOR });
        _slots.push_back(Slot{ "Fac",        "fac",        SlotDirection::OUTPUT, SlotType::FLOAT });
        _slots.push_back(Slot{ "Type",   "interpolation_type", EnumSlotValue{ ImageTexInterpolationType::SHD_INTERP_LINEAR } });
        _slots.push_back(Slot{ "Type",   "projection_type", EnumSlotValue{ ImageTexProjectionType::SHD_PROJ_BOX } });
        _slots.push_back(Slot{ "Vector", "vector",        SlotDirection::INPUT, SlotType::VECTOR ,"VERTEX_CD_MTFACE"});
        _slots.push_back(Slot{ "Image",        "image",   ImageSlotValue{"meshes/spot/spot_texture.png"}});

        _slot_aliases.push_back(std::make_pair("type", "interpolation_type"));
        _slot_aliases.push_back(std::make_pair("type", "projection_type"));
        break;
	case NodeType::MAGIC_TEX:
		_slots.push_back(Slot{ "Color",      "color",      SlotDirection::OUTPUT, SlotType::COLOR });
		_slots.push_back(Slot{ "Fac",        "fac",        SlotDirection::OUTPUT, SlotType::FLOAT });
		_slots.push_back(Slot{ "Depth",      "depth",      IntSlotValue{ 2, 0, 10 } });
		_slots.push_back(Slot{ "Vector",     "vector",     SlotDirection::INPUT, SlotType::VECTOR,"VERTEX_CD_ORCO" });
		_slots.push_back(Slot{ "Scale",      "scale",      FloatSlotValue{ 5.0f, -HALF_FLT_MAX, HALF_FLT_MAX } });
		_slots.push_back(Slot{ "Distortion", "distortion", FloatSlotValue{ 1.0f, -HALF_FLT_MAX, HALF_FLT_MAX } });
		break;
	case NodeType::MUSGRAVE_TEX:
		_slots.push_back(Slot{ "Fac",        "fac",           SlotDirection::OUTPUT, SlotType::FLOAT });
        _slots.push_back(Slot{ "Type",       "musgrave_type", EnumSlotValue{ MusgraveTexType::FBM } });
		_slots.push_back(Slot{ "Dimensions", "dimensions",    EnumSlotValue{ MusgraveTexDimensions::THREE } });
		_slots.push_back(Slot{ "Vector",     "vector",        SlotDirection::INPUT, SlotType::VECTOR,"VERTEX_CD_ORCO" });
		_slots.push_back(Slot{ "W",          "w",             FloatSlotValue{ 0.0f, -HALF_FLT_MAX, HALF_FLT_MAX } });
		_slots.push_back(Slot{ "Scale",      "scale",         FloatSlotValue{ 5.0f, -HALF_FLT_MAX, HALF_FLT_MAX } });
		_slots.push_back(Slot{ "Detail",     "detail",        FloatSlotValue{ 2.0f, 0.0f, 16.0f } });
		_slots.push_back(Slot{ "Dimension",  "dimension",     FloatSlotValue{ 2.0f, 0.0f, HALF_FLT_MAX } });
		_slots.push_back(Slot{ "Lacunarity", "lacunarity",    FloatSlotValue{ 2.0f, 0.0f, HALF_FLT_MAX } });
		_slots.push_back(Slot{ "Offset",     "offset",        FloatSlotValue{ 0.0f, -HALF_FLT_MAX, HALF_FLT_MAX } });
		_slots.push_back(Slot{ "Gain",       "gain",          FloatSlotValue{ 1.0f, 0.0f, HALF_FLT_MAX } });
		_slot_aliases.push_back(std::make_pair("type", "musgrave_type"));
		break;
	case NodeType::NOISE_TEX:
        _slots.push_back(Slot{ "Fac",        "fac",        SlotDirection::OUTPUT, SlotType::FLOAT });
		_slots.push_back(Slot{ "Color",      "color",      SlotDirection::OUTPUT, SlotType::COLOR });
		_slots.push_back(Slot{ "Dimensions", "dimensions", EnumSlotValue{ NoiseTexDimensions::THREE } });
		_slots.push_back(Slot{ "Vector",     "vector",     SlotDirection::INPUT, SlotType::VECTOR ,"VERTEX_CD_ORCO"});
		_slots.push_back(Slot{ "W",          "w",          FloatSlotValue{ 0.0f, -HALF_FLT_MAX, HALF_FLT_MAX } });
		_slots.push_back(Slot{ "Scale",      "scale",      FloatSlotValue{ 5.0f, -HALF_FLT_MAX, HALF_FLT_MAX } });
		_slots.push_back(Slot{ "Detail",     "detail",     FloatSlotValue{ 2.0f, 0.0f, 16.0f } });
		_slots.push_back(Slot{ "Roughness",  "roughness",  FloatSlotValue{ 0.5f, 0.0f, 1.0f } });
		_slots.push_back(Slot{ "Distortion", "distortion", FloatSlotValue{ 0.0f, -HALF_FLT_MAX, HALF_FLT_MAX } });
        _slot_aliases.push_back(std::make_pair("type", "dimensions"));
		break;
	case NodeType::VORONOI_TEX:
		_slots.push_back(Slot{ "Distance",   "distance",   SlotDirection::OUTPUT, SlotType::FLOAT });
		_slots.push_back(Slot{ "Color",      "color",      SlotDirection::OUTPUT, SlotType::COLOR });
		_slots.push_back(Slot{ "Position",   "position",   SlotDirection::OUTPUT, SlotType::VECTOR });
		_slots.push_back(Slot{ "W",          "w",          SlotDirection::OUTPUT, SlotType::FLOAT });
		_slots.push_back(Slot{ "Radius",     "radius",     SlotDirection::OUTPUT, SlotType::FLOAT });
        _slots.push_back(Slot{ "Feature",    "feature",    EnumSlotValue{ VoronoiTexFeature::F1 } });
        _slots.push_back(Slot{ "Dimensions", "dimensions", EnumSlotValue{ VoronoiTexDimensions::THREE } });
		_slots.push_back(Slot{ "Vector",     "vector",     SlotDirection::INPUT, SlotType::VECTOR ,"VERTEX_CD_ORCO"});
		_slots.push_back(Slot{ "W",          "w",          FloatSlotValue{ 0.0f, -HALF_FLT_MAX, HALF_FLT_MAX } });
		_slots.push_back(Slot{ "Scale",      "scale",      FloatSlotValue{ 5.0f, -HALF_FLT_MAX, HALF_FLT_MAX } });
		_slots.push_back(Slot{ "Smoothness", "smoothness", FloatSlotValue{ 1.0f, 0.0f, 1.0f } });
		_slots.push_back(Slot{ "Exponent",   "exponent",   FloatSlotValue{ 0.5f, 0.0f, 32.0f } });
		_slots.push_back(Slot{ "Randomness", "randomness", FloatSlotValue{ 1.0f, 0.0f, 1.0f } });
        _slots.push_back(Slot{ "Metric",     "metric",     EnumSlotValue{ VoronoiTexMetric::EUCLIDEAN } });
		break;
	case NodeType::WAVE_TEX:
		_slots.push_back(Slot{ "Color",            "color",            SlotDirection::OUTPUT, SlotType::COLOR });
		_slots.push_back(Slot{ "Fac",              "fac",              SlotDirection::OUTPUT, SlotType::FLOAT });
		_slots.push_back(Slot{ "Type",             "wave_type",        EnumSlotValue{ WaveTexType::BANDS } });
		_slots.push_back(Slot{ "Direction",        "direction",        EnumSlotValue{ WaveTexDirection::X } });
		_slots.push_back(Slot{ "Profile",          "profile",          EnumSlotValue{ WaveTexProfile::SINE } });
		_slots.push_back(Slot{ "Vector",           "vector",           SlotDirection::INPUT, SlotType::VECTOR ,"VERTEX_CD_ORCO"});
		_slots.push_back(Slot{ "Scale",            "scale",            FloatSlotValue{ 5.0f, -HALF_FLT_MAX, HALF_FLT_MAX } });
		_slots.push_back(Slot{ "Distortion",       "distortion",       FloatSlotValue{ 0.0f, -HALF_FLT_MAX, HALF_FLT_MAX } });
		_slots.push_back(Slot{ "Detail",           "detail",           FloatSlotValue{ 2.0f, 0.0f, 16.0f } });
		_slots.push_back(Slot{ "Detail Scale",     "detail_scale",     FloatSlotValue{ 1.0f, -HALF_FLT_MAX, HALF_FLT_MAX } });
		_slots.push_back(Slot{ "Detail Roughness", "detail_roughness", FloatSlotValue{ 0.5f, 0.0f, 1.0f } });
		_slots.push_back(Slot{ "Phase Offset",     "phase",            FloatSlotValue{ 0.0f, -HALF_FLT_MAX, HALF_FLT_MAX } });
		_slot_aliases.push_back(std::make_pair("type", "wave_type"));
		break;
	case NodeType::WHITE_NOISE_TEX:
		_slots.push_back(Slot{ "Value",      "value",      SlotDirection::OUTPUT, SlotType::FLOAT });
		_slots.push_back(Slot{ "Color",      "color",      SlotDirection::OUTPUT, SlotType::COLOR });
		_slots.push_back(Slot{ "Dimensions", "dimensions", EnumSlotValue{ WhiteNoiseTexDimensions::THREE } });
		_slots.push_back(Slot{ "Vector",     "vector",     SlotDirection::INPUT, SlotType::VECTOR ,"VERTEX_CD_ORCO"});
		_slots.push_back(Slot{ "W",          "w",          FloatSlotValue{ 0.0f, -HALF_FLT_MAX, HALF_FLT_MAX } });
		break;
		//////
		// Vector
		//////
	case NodeType::BUMP:
		_slots.push_back(Slot{ "Normal",   "normal",   SlotDirection::OUTPUT, SlotType::VECTOR });
		_slots.push_back(Slot{ "Invert",   "invert",   BoolSlotValue{ false } });
		_slots.push_back(Slot{ "Strength", "strength", FloatSlotValue{ 1.0f, 0.0f, 1.0f } });
		_slots.push_back(Slot{ "Distance", "distance", FloatSlotValue{ 1.0f, 0.0f, HALF_FLT_MAX } });
		_slots.push_back(Slot{ "Height",   "height",   SlotDirection::INPUT, SlotType::FLOAT });
		_slots.push_back(Slot{ "Normal",   "normal",   SlotDirection::INPUT, SlotType::VECTOR ,"GPU_WORLD_NORMAL"});
		break;
	case NodeType::DISPLACEMENT:
		_slots.push_back(Slot{ "Displacement", "displacement", SlotDirection::OUTPUT, SlotType::VECTOR });
		_slots.push_back(Slot{ "Space",        "space",        EnumSlotValue{ DisplacementSpace::OBJECT } });
		_slots.push_back(Slot{ "Height",       "height",       FloatSlotValue{ 0.0f, 0.0f, HALF_FLT_MAX } });
		_slots.push_back(Slot{ "Midlevel",     "midlevel",     FloatSlotValue{ 0.5f, 0.0f, HALF_FLT_MAX } });
		_slots.push_back(Slot{ "Scale",        "scale",        FloatSlotValue{ 1.0f, 0.0f, HALF_FLT_MAX } });
		_slots.push_back(Slot{ "Normal",       "normal",       SlotDirection::INPUT, SlotType::VECTOR,"GPU_WORLD_NORMAL" });
		break;
	case NodeType::MAPPING:
		_slots.push_back(Slot{ "Vector",   "vector",       SlotDirection::OUTPUT, SlotType::VECTOR });
		_slots.push_back(Slot{ "Type",     "mapping_type", EnumSlotValue{ VectorMappingType::POINT } });
		_slots.push_back(Slot{ "Vector",   "vector",       VectorSlotValue{
			csc::Float3{ 0.0f, 0.0f, 0.0f }, csc::Float3{ -HALF_FLT_MAX, -HALF_FLT_MAX, -HALF_FLT_MAX } , csc::Float3{ HALF_FLT_MAX, HALF_FLT_MAX, HALF_FLT_MAX }
		} });
		_slots.push_back(Slot{ "Location", "location",     VectorSlotValue{
			csc::Float3{ 0.0f, 0.0f, 0.0f }, csc::Float3{ -HALF_FLT_MAX, -HALF_FLT_MAX, -HALF_FLT_MAX } , csc::Float3{ HALF_FLT_MAX, HALF_FLT_MAX, HALF_FLT_MAX }
		} });
		_slots.push_back(Slot{ "Rotation", "rotation",     VectorSlotValue{
			csc::Float3{ 0.0f, 0.0f, 0.0f }, csc::Float3{ -HALF_FLT_MAX, -HALF_FLT_MAX, -HALF_FLT_MAX } , csc::Float3{ HALF_FLT_MAX, HALF_FLT_MAX, HALF_FLT_MAX }
		} });
		_slots.push_back(Slot{ "Scale", "scale",           VectorSlotValue{
			csc::Float3{ 1.0f, 1.0f, 1.0f }, csc::Float3{ -HALF_FLT_MAX, -HALF_FLT_MAX, -HALF_FLT_MAX } , csc::Float3{ HALF_FLT_MAX, HALF_FLT_MAX, HALF_FLT_MAX }
		} });
		_slot_aliases.push_back(std::make_pair("type", "mapping_type"));
		break;
	case NodeType::NORMAL:
		_slots.push_back(Slot{ "Normal",    "normal",    SlotDirection::OUTPUT, SlotType::VECTOR });
		_slots.push_back(Slot{ "Dot",       "dot",       SlotDirection::OUTPUT, SlotType::FLOAT });
		_slots.push_back(Slot{ "Direction", "direction", VectorSlotValue{
			csc::Float3{ 0.0f, 0.0f, 0.0f },  csc::Float3{ -HALF_FLT_MAX, -HALF_FLT_MAX, -HALF_FLT_MAX } , csc::Float3{ HALF_FLT_MAX, HALF_FLT_MAX, HALF_FLT_MAX }
		}, false });
		_slots.push_back(Slot{ "Normal",    "normal",    VectorSlotValue{
			csc::Float3{ 0.0f, 0.0f, 0.0f }, csc::Float3{ -HALF_FLT_MAX, -HALF_FLT_MAX, -HALF_FLT_MAX } , csc::Float3{ HALF_FLT_MAX, HALF_FLT_MAX, HALF_FLT_MAX }
		} });
		break;
	case NodeType::NORMAL_MAP:
		_slots.push_back(Slot{ "Normal",   "normal",   SlotDirection::OUTPUT, SlotType::VECTOR });
		_slots.push_back(Slot{ "Space",    "space",    EnumSlotValue{ NormalMapSpace::TANGENT } });
		_slots.push_back(Slot{ "Strength", "strength", FloatSlotValue{ 1.0f, 0.0f, 10.0f } });
		_slots.push_back(Slot{ "Color",    "color",    ColorSlotValue{ csc::Float3{ 0.5f, 0.5f, 1.0f} } });
		break;
	case NodeType::VECTOR_CURVES:
		_slots.push_back(Slot{ "Vector", "vector", SlotDirection::OUTPUT, SlotType::VECTOR });
		_slots.push_back(Slot{ "Curves", "curves", VectorCurveSlotValue{ csc::Float2{ -1.0f, -1.0f }, csc::Float2{ 1.0f, 1.0f} } });
		_slots.push_back(Slot{ "Fac",    "fac",    FloatSlotValue{ 1.0f, 0.0f, 1.0f } });
		_slots.push_back(Slot{ "Vector", "vector", VectorSlotValue{
			csc::Float3{ 0.0f, 0.0f, 0.0f }, csc::Float3{ -HALF_FLT_MAX, -HALF_FLT_MAX, -HALF_FLT_MAX } , csc::Float3{ HALF_FLT_MAX, HALF_FLT_MAX, HALF_FLT_MAX }
		} });
		break;
	case NodeType::VECTOR_DISPLACEMENT:
		_slots.push_back(Slot{ "Displacement", "displacement", SlotDirection::OUTPUT, SlotType::VECTOR });
		_slots.push_back(Slot{ "Space",        "space",        EnumSlotValue{ VectorDisplacementSpace::TANGENT } });
		_slots.push_back(Slot{ "Vector",       "vector",       SlotDirection::INPUT, SlotType::VECTOR ,"GPU_WORLD_NORMAL"});
		_slots.push_back(Slot{ "Midlevel",     "midlevel",     FloatSlotValue{ 0.0f, 0.0f, HALF_FLT_MAX } });
		_slots.push_back(Slot{ "Scale",        "scale",        FloatSlotValue{ 1.0f, 0.0f, HALF_FLT_MAX } });
		break;
	case NodeType::VECTOR_TRANSFORM:
		_slots.push_back(Slot{ "Vector",       "vector",         SlotDirection::OUTPUT, SlotType::VECTOR });
		_slots.push_back(Slot{ "Type",         "transform_type", EnumSlotValue{ VectorTransformType::VECTOR } });
		_slots.push_back(Slot{ "Convert From", "convert_from",   EnumSlotValue{ VectorTransformSpace::WORLD } });
		_slots.push_back(Slot{ "Convert To",   "convert_to",     EnumSlotValue{ VectorTransformSpace::OBJECT } });
		_slots.push_back(Slot{ "Vector",       "vector",         VectorSlotValue{
			csc::Float3{ 1.0f, 1.0f, 1.0f }, csc::Float3{ -HALF_FLT_MAX, -HALF_FLT_MAX, -HALF_FLT_MAX } , csc::Float3{ HALF_FLT_MAX, HALF_FLT_MAX, HALF_FLT_MAX }
		} });
		_slot_aliases.push_back(std::make_pair("type", "transform_type"));
		break;
	default:
		// Uncomment the below assert once all node types have been implemented
		assert(false);
	}
	roll_id();
}

csg::Node::Node(const NodeType type, const csc::Int2 position, const NodeId id) :
	Node(type, position)
{
	_id = id;
}

boost::optional<size_t> csg::Node::slot_index(const SlotDirection dir, const boost::string_view& slot_name) const
{
	for (size_t i = 0; i < _slots.size(); i++)
	{
		if (_slots[i].dir() == dir && _slots[i].name() == slot_name) {
			return i;
		}
	}
	for (const auto& this_pair : _slot_aliases) {
		if (slot_name == this_pair.first) {
			return slot_index(dir, this_pair.second);
		}
	}
	return boost::none;
}

boost::optional<csg::Slot> csg::Node::slot(const size_t index) const
{
	if (index >= _slots.size()) {
		return boost::none;
	}
	return _slots.at(index);
}

boost::optional<csg::Slot> csg::Node::slot(const SlotDirection dir, const boost::string_view& slot_name) const
{
	const boost::optional<size_t> opt_index{ slot_index(dir, slot_name) };
	if (opt_index) {
		return slot(*opt_index);
	}
	else {
		return boost::none;
	}
}

boost::optional<csg::SlotValue> csg::Node::slot_value(const size_t index) const
{
	const boost::optional<csg::Slot> the_slot{ slot(index) };
	if (the_slot) {
		return the_slot->value;
	}
	else {
		return boost::none;
	}
}

boost::optional <csg::SlotValue> csg::Node::slot_value(const boost::string_view& slot_name) const
{
	const boost::optional<size_t> opt_index{ slot_index(csg::SlotDirection::INPUT, slot_name) };
	if (opt_index) {
		return slot_value(*opt_index);
	}
	else {
		return boost::none;
	}
}

void csg::Node::copy_from(const Node& other)
{
	// Copy everything except id
	_type = other._type;
	_slots = other._slots;
}

bool csg::Node::operator==(const Node& other) const
{
	if (id() != other.id()) {
		return false;
	}

	if (type() != other.type()) {
		return false;
	}

	if (position != other.position) {
		return false;
	}

	if (_slots.size() != other._slots.size()) {
		return false;
	}

	for (size_t i = 0; i < _slots.size(); i++) {
		if (_slots[i] != other._slots[i]) {
			return false;
		}
	}

	return true;
}

csg::NodeId csg::Node::roll_id()
{
	static_assert(sizeof(uint32_t) * 2 == sizeof(NodeId), "NodeId should be twice the size of uint32_t");
	uint32_t* const id_ptr{ reinterpret_cast<uint32_t*>(&_id) };
	std::lock_guard<std::mutex> lock{ node_id_rng_mutex };
	id_ptr[0] = node_id_rng();
	id_ptr[1] = node_id_rng();
	return _id;
}
