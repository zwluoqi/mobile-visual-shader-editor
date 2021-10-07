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

uniform vec3 vector_math4_vector1_3;
uniform vec3 vector_math4_vector2_4;
uniform vec3 vector_math4_vector3_5;
uniform float vector_math4_scale_6;
uniform float combine_rgb3_g_2;
uniform float combine_rgb3_b_3;
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
 vec4 combine_rgb3_image_0;
 float separate_xyz1_x_0;
 float separate_xyz1_y_1;
 float separate_xyz1_z_2;
 vec3 vector_math4_vector_0;
 float vector_math4_value_1;
 vector_math_reflect(vector_math4_vector1_3,vector_math4_vector2_4,vector_math4_vector3_5,vector_math4_scale_6,vector_math4_vector_0,vector_math4_value_1);
 node_separate_xyz(vector_math4_vector_0,separate_xyz1_x_0,separate_xyz1_y_1,separate_xyz1_z_2);
 node_combine_rgb(separate_xyz1_x_0,combine_rgb3_g_2,combine_rgb3_b_3,combine_rgb3_image_0);
 vec4 emission0_emission_0=emission0_strength_2*vec4(lightRadiance,1.0)*combine_rgb3_image_0;
 gl_FragColor = emission0_emission_0;

}
