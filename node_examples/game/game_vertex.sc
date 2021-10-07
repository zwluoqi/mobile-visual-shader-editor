//--------
//this vertex code generate by zhouxingxing's visual shader scirpt app

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

$input  a_position, a_texcoord0, a_normal, a_tangent // in...

$output  GPU_WORLD_POSITION, VERTEX_CD_MTFACE, VERTEX_CD_ORCO, GPU_WORLD_NORMAL, VERTEX_CD_TANGENT, vWorldToLight, vPositionFromLight // out...

#include "../common/common.sh"
#include "../node/include/share_uniform.cginc"

void main(){
 GPU_WORLD_POSITION = mul(u_model[0] , vec4(a_position, 1.0)).xyz;
 VERTEX_CD_MTFACE = vec3(a_texcoord0,0.0);
 VERTEX_CD_ORCO = a_position;
 gl_Position = mul(u_modelViewProj ,vec4(a_position, 1.0));
 GPU_WORLD_NORMAL = mul(transpose((u_model[1])) , vec4(a_normal, 0.0)).xyz;
 VERTEX_CD_TANGENT = a_tangent;
 vWorldToLight = uLightVP;
 vPositionFromLight = uLightMVP * vec4(a_position, 1.0);

}
