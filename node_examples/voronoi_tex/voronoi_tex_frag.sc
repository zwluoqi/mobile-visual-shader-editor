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

uniform float voronoi_tex1_w_8;
uniform float voronoi_tex1_scale_9;
uniform float voronoi_tex1_smoothness_10;
uniform float voronoi_tex1_exponent_11;
uniform float voronoi_tex1_randomness_12;
uniform float voronoi_tex1_metric_13;
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
 float voronoi_tex1_distance_0;
 vec4 voronoi_tex1_color_1;
 vec3 voronoi_tex1_position_2;
 float voronoi_tex1_w_3;
 float voronoi_tex1_radius_4;
 f1_3d_node_tex_voronoi(VERTEX_CD_ORCO,voronoi_tex1_w_8,voronoi_tex1_scale_9,voronoi_tex1_smoothness_10,voronoi_tex1_exponent_11,voronoi_tex1_randomness_12,voronoi_tex1_metric_13,voronoi_tex1_distance_0,voronoi_tex1_color_1,voronoi_tex1_position_2,voronoi_tex1_w_3,voronoi_tex1_radius_4);
 vec4 emission0_emission_0=emission0_strength_2*vec4(lightRadiance,1.0)*voronoi_tex1_color_1;
 gl_FragColor = emission0_emission_0;

}
