//--------
//this fragment code generate by zhouxingxing's visual shader scirpt app

    //{
    //    "u_viewRect",
    //    "u_viewTexel",
    //    "u_view",
    //    "u_invView",
    //    "u_proj",
    //    "u_invProj",
    //    "u_viewProj",
    //    "u_invViewProj",
    //    "u_model",array
    //    "u_modelView",
    //    "u_modelViewProj",
    //    "u_alphaRef4",
    //};
//Version 1.0
//--------

$input  GPU_WORLD_POSITION, VERTEX_CD_MTFACE, VERTEX_CD_ORCO GPU_WORLD_NORMAL vWorldToLight vPositionFromLight // in...

#include "../common/common.sh"
#include "../node/node.cginc"

uniform float image_tex1_projection_type_3;
SAMPLER2D(image_tex1_image_5,0);
uniform float diffuse_bsdf0_roughness_2;
uniform float bump6_invert_1;
uniform float bump6_strength_2;
uniform float bump6_distance_3;
uniform vec4 checker_tex7_color1_3;
uniform vec4 checker_tex7_color2_4;
uniform float checker_tex7_scale_5;
uniform float emission8_strength_2;
uniform vec4 out_material4_volume_1;
uniform vec3 out_material4_displacement_2;





//----------------start generate api----------------
//----------------end generate api----------------




void main(){
vec3 viewDir = mul(u_view , vec4(GPU_WORLD_POSITION,1.0));
vec4 colorRamp = texture(ramp_curve,VERTEX_CD_MTFACE);
vec3 GPU_VIEW_POSITION = mul(u_view , vec4(GPU_WORLD_POSITION, 1.0)).xyz;
vec3 lightDir = normalize(uLightDir.xyz);
vec3 lightRadiance = uLightRadiance.xyz;
 vec4 diffuse_bsdf0_BSDF_0;
 vec4 image_tex1_color_0;
 float image_tex1_fac_1;
 node_tex_image_linear_simple(image_tex1_projection_type_3,VERTEX_CD_MTFACE,image_tex1_image_5,image_tex1_color_0,image_tex1_fac_1);
 vec3 bump6_normal_0;
 vec4 checker_tex7_color_0;
 float checker_tex7_fac_1;
 node_tex_checker(VERTEX_CD_ORCO,checker_tex7_color1_3,checker_tex7_color2_4,checker_tex7_scale_5,checker_tex7_color_0,checker_tex7_fac_1);
 node_bump(bump6_invert_1,bump6_strength_2,bump6_distance_3,checker_tex7_fac_1,GPU_WORLD_NORMAL,GPU_VIEW_POSITION,bump6_normal_0);
 node_bsdf_orenNayar(image_tex1_color_0,diffuse_bsdf0_roughness_2,bump6_normal_0,GPU_WORLD_POSITION,diffuse_bsdf0_BSDF_0);
 vec4 emission8_emission_0;
 node_bsdf_emission(image_tex1_color_0,emission8_strength_2,emission8_emission_0);
 vec4 add_shader5_closure_0=diffuse_bsdf0_BSDF_0+emission8_emission_0;
 gl_FragColor = add_shader5_closure_0;

}
