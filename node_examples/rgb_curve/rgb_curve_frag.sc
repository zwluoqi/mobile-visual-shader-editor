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

uniform float rgb_curves1_curves_1;
uniform float rgb_curves1_fac_2;
uniform vec4 rgb_curves1_color_3;
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
 vec4 rgb_curves1_color_0;
vec4 rgb_curves1_curves_1_range=vec4(1,1,1,1);;
vec4 rgb_curves1_curves_1_ext_x=vec4(0,0,1,0);;
vec4 rgb_curves1_curves_1_ext_y=vec4(0,0,1,0);;
vec4 rgb_curves1_curves_1_ext_z=vec4(0,0,1,0);;
vec4 rgb_curves1_curves_1_ext_w=vec4(0,0,1,0);;
 curves_rgb(rgb_curves1_curves_1,rgb_curves1_fac_2,rgb_curves1_color_3,rgb_curves1_curves_1_range,rgb_curves1_curves_1_ext_x,rgb_curves1_curves_1_ext_y,rgb_curves1_curves_1_ext_z,rgb_curves1_curves_1_ext_w,rgb_curves1_color_0);
 vec4 emission0_emission_0=emission0_strength_2*vec4(lightRadiance,1.0)*rgb_curves1_color_0;
 gl_FragColor = emission0_emission_0;

}
