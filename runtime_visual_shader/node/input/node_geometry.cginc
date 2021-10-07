/*
* Copyright 2021-2021 Zhouwei. All rights reserved.
* License: https://github.com/zwluoqi/mobile-visual-shader-editor#license-bsd-2-clause
*/


#ifndef node_geometry_api
#define node_geometry_api
//#include "UnityCG.cginc"
#include "../include/base_h.cginc"
/*

*/
void node_geometry(vec3 GPU_VIEW_POSITION,//视空间顶点
                   vec3 GPU_WORLD_NORMAL,//世界空间法线
                   vec3 VERTEX_CD_ORCO,//
                   vec2 GPU_BARYCENTRIC_TEXCO,//重心坐标uv 需要几何着色器支持
                   vec3 GPU_WORLD_POSITION ,
                   out vec3 position,
                   out vec3 normal,
                   out vec3 tangent,
                   out vec3 true_normal,
                   out vec3 incoming,//视空间着色方向
                   out vec3 parametric,
                   out float backfacing,
                   out float pointiness,
                   out float random_per_island)
{
  /* handle perspective/orthographic */
  vec3 I_view = (ProjectionMatrix[3][3] == 0.0) ? normalize(GPU_VIEW_POSITION) : vec3(0.0, 0.0, -1.0);
  incoming = -mul(view2worldmat , vec4(I_view, 0.0)).xyz;


  position = GPU_WORLD_POSITION;

  normal = normalize(GPU_WORLD_NORMAL);
  vec3 B = dFdx(GPU_WORLD_POSITION);
  vec3 T = dFdy(GPU_WORLD_POSITION);
  true_normal = normalize(cross(B, T));

  tangent_orco_z(VERTEX_CD_ORCO, VERTEX_CD_ORCO);
  tangent_translate(GPU_WORLD_NORMAL, VERTEX_CD_ORCO, objmat, tangent);

  parametric = vec3(GPU_BARYCENTRIC_TEXCO, 0.0);
  backfacing =  0.0;
  pointiness = 0.5;
  random_per_island = 0.0;

}

#endif
