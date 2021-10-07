/*
* Copyright 2021-2021 Zhouwei. All rights reserved.
* License: https://github.com/zwluoqi/mobile-visual-shader-editor#license-bsd-2-clause
*/


#ifndef node_tex_coord_api
#define node_tex_coord_api
//#include "UnityCG.cginc"
#include "../include/base_h.cginc"
/*

*/
//区间变化,[-1,1]转到[0,1]
vec3 mtex_2d_mapping(vec3 vec)
{
  return vec3(vec.xy * 0.5 + vec2(0.5,0.5), vec.z);
}

void generated_texco(vec3 GPU_VIEW_POSITION, vec3 VERTEX_CD_ORCO, out vec3 generated)
{
/*
  mat4 ProjectionMatrixInverse;
  invertmat4(ProjectionMatrix,ProjectionMatrixInverse);
  vec4 v = (ProjectionMatrix[3][3] == 0.0) ? vec4(GPU_VIEW_POSITION, 1.0) : vec4(0.0, 0.0, 1.0, 1.0);
  vec4 co_homogenous = mul(ProjectionMatrixInverse , v);
  vec4 co = vec4(co_homogenous.xyz / co_homogenous.w, 0.0);
  co.xyz = normalize(co.xyz);
#if defined(WORLD_BACKGROUND) || defined(PROBE_CAPTURE)
  generated = (ViewMatrixInverse * co).xyz;
#else
  generated = VERTEX_CD_ORCO;
#endif
*/
    //没有background模式,直接返回顶点坐标

  generated = VERTEX_CD_ORCO;

}

void node_tex_coord(vec3 GPU_VIEW_POSITION,
                    vec3 GPU_WORLD_NORMAL,
                    vec4 GPU_CAMERA_TEXCO_FACTORS,
                    vec3 VERTEX_CD_ORCO,
                    vec3 VERTEX_CD_MTFACE,
                    vec3 GPU_WORLD_POSITION ,
                    out vec3 generated,//顶点原始的坐标系
                    out vec3 normal,//对象空间法线
                    out vec3 uv,//顶点UV MAP 暂时不知道怎么用
                    out vec3 object,//对象空间顶点坐标
                    out vec3 camera,//视空间顶点方向
                    out vec3 window,//[0,1]的NDC坐标
                    out vec3 reflection)//世界空间反射方向
{
  mat4 ModelMatrixInverse;
  invertmat4(ModelMatrix,ModelMatrixInverse);

  generated = VERTEX_CD_ORCO;
  normal_transform_transposed_m4v3(GPU_WORLD_NORMAL,ModelMatrixInverse,normal);
  normal = normalize(normal);
  
  uv = VERTEX_CD_MTFACE;
  object = mul(ModelMatrixInverse , mul(ViewMatrixInverse , vec4(GPU_VIEW_POSITION, 1.0))).xyz;
  camera = vec3(GPU_VIEW_POSITION.xy, -GPU_VIEW_POSITION.z);
  vec4 projvec = mul(ProjectionMatrix , vec4(GPU_VIEW_POSITION, 1.0));
  window = vec3(mtex_2d_mapping(projvec.xyz / projvec.w).xy * GPU_CAMERA_TEXCO_FACTORS.xy + GPU_CAMERA_TEXCO_FACTORS.zw, 0.0);
  reflection = -reflect(cameraVec(GPU_WORLD_POSITION), normalize(GPU_WORLD_NORMAL));
}

#endif
