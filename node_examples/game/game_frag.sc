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

$input  GPU_WORLD_POSITION, VERTEX_CD_MTFACE, VERTEX_CD_ORCO, GPU_WORLD_NORMAL VERTEX_CD_TANGENT vWorldToLight vPositionFromLight // in...

#include "../common/common.sh"
#include "../node/node.cginc"

uniform float color_ramp2_ramp_2;
uniform float color_ramp2_fac_3;
uniform float gamma0_gamma_2;
uniform float emission1_strength_2;





//----------------start generate api----------------
//----------------end generate api----------------




void main(){
vec3 viewDir = mul(u_view , vec4(GPU_WORLD_POSITION,1.0));
vec4 colorRamp = texture(ramp_curve,VERTEX_CD_MTFACE);
vec3 GPU_VIEW_POSITION = mul(u_view , vec4(GPU_WORLD_POSITION, 1.0)).xyz;
vec2 GPU_BARYCENTRIC_TEXCO=vec2(0.0,0.0);
vec3 GPU_BARYCENTRIC_DIST=vec3(0.0,0.0,0.0);
vec4 GPU_CAMERA_TEXCO_FACTORS = vec4(1.0,1.0,0.0,0.0);
vec3 lightDir = normalize(uLightDir.xyz);
vec3 lightRadiance = uLightRadiance.xyz;
 Closure emission1_emission_0;
 vec4 gamma0_color_0;
 vec4 color_ramp2_color_0;
 float color_ramp2_alpha_1;
valtorgb(color_ramp2_fac_3,ramp_curve,color_ramp2_ramp_2,color_ramp2_color_0,color_ramp2_alpha_1);
 node_gamma(color_ramp2_color_0,gamma0_gamma_2,gamma0_color_0);
 node_bsdf_emission(gamma0_color_0,emission1_strength_2,emission1_emission_0);
 gl_FragColor = emission1_emission_0.radiance;

}
