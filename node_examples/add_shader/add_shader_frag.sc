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

uniform float image_tex1_projection_type_3;
SAMPLER2D(image_tex1_image_5,0);
uniform vec4 rgb_to_bw2_color_1;





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
 Closure add_shader3_closure_0;
 Closure emission0_emission_0;
 vec4 image_tex1_color_0;
 float image_tex1_fac_1;
 node_tex_image_linear_simple(image_tex1_projection_type_3,VERTEX_CD_MTFACE,image_tex1_image_5,image_tex1_color_0,image_tex1_fac_1);
 float rgb_to_bw2_val_0;
 node_rgbtobw(rgb_to_bw2_color_1,rgb_to_bw2_val_0);
 node_bsdf_emission(image_tex1_color_0,rgb_to_bw2_val_0,emission0_emission_0);
 node_add_shader(emission0_emission_0,emission0_emission_0,add_shader3_closure_0);
 gl_FragColor = add_shader3_closure_0.radiance;

}
