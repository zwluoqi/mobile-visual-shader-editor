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

uniform float image_tex2_projection_type_3;
SAMPLER2D(image_tex2_image_5,0);
uniform vec4 pbr1_specular_5;
uniform float pbr1_occlusion_6;
uniform vec4 pbr1_emission_7;
uniform float pbr1_smoothness_8;





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
 Closure pbr1_BSDF_0;
 vec4 pbr1_diffuse_1;
 vec4 pbr1_specular_2;
 vec4 pbr1_surface_reduction_3;
 vec4 image_tex2_color_0;
 float image_tex2_fac_1;
 node_tex_image_linear_simple(image_tex2_projection_type_3,VERTEX_CD_MTFACE,image_tex2_image_5,image_tex2_color_0,image_tex2_fac_1);
 node_bsdf_pbr(image_tex2_color_0,pbr1_specular_5,pbr1_occlusion_6,pbr1_emission_7,pbr1_smoothness_8,GPU_WORLD_NORMAL,GPU_WORLD_POSITION,pbr1_BSDF_0,pbr1_diffuse_1,pbr1_specular_2,pbr1_surface_reduction_3);
 gl_FragColor = pbr1_BSDF_0.radiance;

}
