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
uniform vec3 vector_transform4_vector_4;





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
 Closure emission0_emission_0;
 vec4 image_tex1_color_0;
 float image_tex1_fac_1;
 node_tex_image_linear_simple(image_tex1_projection_type_3,VERTEX_CD_MTFACE,image_tex1_image_5,image_tex1_color_0,image_tex1_fac_1);
 float separate_xyz3_x_0;
 float separate_xyz3_y_1;
 float separate_xyz3_z_2;
 vec3 vector_transform4_vector_0;
 transform_point_object_world(vector_transform4_vector_4,vector_transform4_vector_0);
 node_separate_xyz(vector_transform4_vector_0,separate_xyz3_x_0,separate_xyz3_y_1,separate_xyz3_z_2);
 node_bsdf_emission(image_tex1_color_0,separate_xyz3_x_0,emission0_emission_0);
 gl_FragColor = emission0_emission_0.radiance;

}
