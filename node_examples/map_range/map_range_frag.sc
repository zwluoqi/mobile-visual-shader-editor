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

$input  GPU_WORLD_POSITION, VERTEX_CD_MTFACE, GPU_WORLD_NORMAL vWorldToLight vPositionFromLight // in...

#include "../common/common.sh"
#include "../node/node.cginc"

uniform vec4 emission0_color_1;
uniform float map_range1_value_2;
uniform float map_range1_from_min_3;
uniform float map_range1_from_max_4;
uniform float map_range1_to_min_5;
uniform float map_range1_to_max_6;
uniform float map_range1_steps_7;
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
 float map_range1_result_0;
 map_range_linear(map_range1_value_2,map_range1_from_min_3,map_range1_from_max_4,map_range1_to_min_5,map_range1_to_max_6,map_range1_steps_7,map_range1_result_0);
 vec4 emission0_emission_0=map_range1_result_0*vec4(lightRadiance,1.0)*emission0_color_1;
 gl_FragColor = emission0_emission_0;

}
