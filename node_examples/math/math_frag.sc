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
uniform vec4 rgb_to_bw5_color_1;
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
 float rgb_to_bw5_val_0=rgb_to_bw5_color_1.x*0.299+rgb_to_bw5_color_1.y*0.587+rgb_to_bw5_color_1.z*0.114
 vec4 emission0_emission_0=rgb_to_bw5_val_0*vec4(lightRadiance,1.0)*emission0_color_1;
 gl_FragColor = emission0_emission_0;

}
