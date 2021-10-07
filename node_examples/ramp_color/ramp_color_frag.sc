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

uniform float hsv3_hue;
uniform float hsv3_saturation;
uniform float hsv3_value;
uniform float hsv3_fac;
uniform float color_ramp1_fac;
uniform float emission0_strength;
uniform vec4 out_material2_volume;
uniform vec3 out_material2_displacement;





//----------------start generate api----------------
//----------------end generate api----------------




void main(){
vec3 viewDir = mul(u_view , vec4(GPU_WORLD_POSITION,1.0));
vec4 colorRamp = texture(ramp_curve,VERTEX_CD_MTFACE);
vec3 normal = normalize(GPU_WORLD_NORMAL);
vec3 lightDir = normalize(uLightDir.xyz);
vec3 lightRadiance = uLightRadiance.xyz;
 vec4 hsv3_color;
float color_ramp1_ramp=0;
 vec4 color_ramp1_color;
 float color_ramp1_alpha;
valtorgb(color_ramp1_fac,ramp_curve,color_ramp1_ramp,color_ramp1_color,color_ramp1_alpha);
 node_hsv(hsv3_hue,hsv3_saturation,hsv3_value,hsv3_fac,color_ramp1_color,hsv3_color);
 vec4 emission0_emission=emission0_strength*vec4(lightRadiance,1.0)*hsv3_color;
 gl_FragColor = emission0_emission;

}
