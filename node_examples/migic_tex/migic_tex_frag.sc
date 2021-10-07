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

uniform float magic_tex1_depth_2;
uniform float magic_tex1_scale_4;
uniform float magic_tex1_distortion_5;
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
 vec4 magic_tex1_color_0;
 float magic_tex1_fac_1;
 node_tex_magic(magic_tex1_depth_2,VERTEX_CD_ORCO,magic_tex1_scale_4,magic_tex1_distortion_5,magic_tex1_color_0,magic_tex1_fac_1);
 vec4 emission0_emission_0=emission0_strength_2*vec4(lightRadiance,1.0)*magic_tex1_color_0;
 gl_FragColor = emission0_emission_0;

}
