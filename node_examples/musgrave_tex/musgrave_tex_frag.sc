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

uniform vec4 emission0_color_1;
uniform float musgrave_tex1_w_4;
uniform float musgrave_tex1_scale_5;
uniform float musgrave_tex1_detail_6;
uniform float musgrave_tex1_dimension_7;
uniform float musgrave_tex1_lacunarity_8;
uniform float musgrave_tex1_offset_9;
uniform float musgrave_tex1_gain_10;
uniform float map_range3_from_min_3;
uniform float map_range3_from_max_4;
uniform float map_range3_to_min_5;
uniform float map_range3_to_max_6;
uniform float map_range3_steps_7;
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
 float map_range3_result_0;
 float musgrave_tex1_fac_0;
 node_tex_musgrave_fBm_3d(VERTEX_CD_ORCO,musgrave_tex1_w_4,musgrave_tex1_scale_5,musgrave_tex1_detail_6,musgrave_tex1_dimension_7,musgrave_tex1_lacunarity_8,musgrave_tex1_offset_9,musgrave_tex1_gain_10,musgrave_tex1_fac_0);
 map_range_linear(musgrave_tex1_fac_0,map_range3_from_min_3,map_range3_from_max_4,map_range3_to_min_5,map_range3_to_max_6,map_range3_steps_7,map_range3_result_0);
 vec4 emission0_emission_0=map_range3_result_0*vec4(lightRadiance,1.0)*emission0_color_1;
 gl_FragColor = emission0_emission_0;

}
