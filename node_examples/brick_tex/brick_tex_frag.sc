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

uniform float brick_tex1_offset_2;
uniform float brick_tex1_offset_frequency_3;
uniform float brick_tex1_squash_4;
uniform float brick_tex1_squash_frequency_5;
uniform vec4 brick_tex1_color1_7;
uniform vec4 brick_tex1_color2_8;
uniform vec4 brick_tex1_mortar_9;
uniform float brick_tex1_scale_10;
uniform float brick_tex1_mortar_size_11;
uniform float brick_tex1_mortar_smooth_12;
uniform float brick_tex1_bias_13;
uniform float brick_tex1_brick_width_14;
uniform float brick_tex1_row_height_15;
uniform float emission0_strength_2;
uniform vec4 out_material2_volume_1;
uniform vec3 out_material2_displacement_2;





//----------------start generate api----------------
//----------------end generate api----------------




void main(){
vec3 viewDir = mul(u_view , vec4(GPU_WORLD_POSITION,1.0));
vec4 colorRamp = texture(ramp_curve,VERTEX_CD_MTFACE);
vec3 normal = normalize(GPU_WORLD_NORMAL);
vec3 lightDir = normalize(uLightDir.xyz);
vec3 lightRadiance = uLightRadiance.xyz;
 vec4 brick_tex1_color_0;
 float brick_tex1_fac_1;
 node_tex_brick(brick_tex1_offset_2,brick_tex1_offset_frequency_3,brick_tex1_squash_4,brick_tex1_squash_frequency_5,VERTEX_CD_ORCO,brick_tex1_color1_7,brick_tex1_color2_8,brick_tex1_mortar_9,brick_tex1_scale_10,brick_tex1_mortar_size_11,brick_tex1_mortar_smooth_12,brick_tex1_bias_13,brick_tex1_brick_width_14,brick_tex1_row_height_15,brick_tex1_color_0,brick_tex1_fac_1);
 vec4 emission0_emission_0=emission0_strength_2*vec4(lightRadiance,1.0)*brick_tex1_color_0;
 gl_FragColor = emission0_emission_0;

}
