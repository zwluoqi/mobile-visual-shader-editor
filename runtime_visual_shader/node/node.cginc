

#ifndef node
#define node
//#include "UnityCG.cginc"

#include "color/node_brightness_contrast.cginc"
#include "color/node_curves_rgb.cginc"
#include "color/node_hsv.cginc"
#include "color/node_gamma.cginc"
#include "color/node_invert.cginc"
#include "color/node_light_falloff.cginc"
#include "color/node_mix.cginc"


#include "converter/node_blackbody.cginc"
#include "converter/node_combine_hsv.cginc"
#include "converter/node_combine_rgb.cginc"
#include "converter/node_combine_xyz.cginc"
#include "converter/node_map_range.cginc"
#include "converter/node_math.cginc"
#include "converter/node_rgbtobw.cginc"
#include "converter/node_separate_hsv.cginc"
#include "converter/node_separate_rgb.cginc"
#include "converter/node_separate_xyz.cginc"
#include "converter/node_vector_math.cginc"
#include "converter/node_color_ramp.cginc"
#include "converter/node_clamp.cginc"
#include "converter/node_wavelength.cginc"

#include "shader/node_bsdf_oren_nayar.cginc"
#include "shader/node_bsdf_emission.cginc"
#include "shader/node_bsdf_surface.cginc"
#include "shader/node_add_shader.cginc"
#include "shader/node_mix_shader.cginc"

#include "texture/node_tex_checker.cginc"
#include "texture/node_tex_brick.cginc"
#include "texture/node_tex_gradient.cginc"
#include "texture/node_tex_image.cginc"
#include "texture/node_tex_magic.cginc"
#include "texture/node_tex_musgrave.cginc"
#include "texture/node_tex_noise.cginc"
#include "texture/node_tex_voronoi.cginc"
#include "texture/node_tex_wave.cginc"
#include "texture/node_tex_white_noise.cginc"


#include "vector/node_bump.cginc"
#include "vector/node_displacement.cginc"
#include "vector/node_mapping.cginc"
#include "vector/node_normal.cginc"
#include "vector/node_normal_map.cginc"
#include "vector/node_vector_curves.cginc"
#include "vector/node_vector_displacement.cginc"
#include "vector/node_vector_rotate.cginc"
#include "vector/node_vector_transform.cginc"



#include "input/node_bevel.cginc"
#include "input/node_camera.cginc"
#include "input/node_fresnel.cginc"
#include "input/node_geometry.cginc"
#include "input/node_layer_weight.cginc"
#include "input/node_light_path.cginc"
#include "input/node_object_info.cginc"
#include "input/node_rgb.cginc"
#include "input/node_tangent.cginc"
#include "input/node_tex_coord.cginc"
#include "input/node_value.cginc"
#include "input/node_wireframe.cginc"



#endif
