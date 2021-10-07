/*
* Copyright 2021-2021 Zhouwei. All rights reserved.
* License: https://github.com/zwluoqi/mobile-visual-shader-editor#license-bsd-2-clause
*/


#ifndef node_wireframe_api
#define node_wireframe_api
//#include "UnityCG.cginc"
#include "../include/base_h.cginc"



void node_wireframe_worldspace(float size, vec2 GPU_BARYCENTRIC_TEXCO, vec3 GPU_BARYCENTRIC_DIST, out float fac)
{
  vec3 barys = GPU_BARYCENTRIC_TEXCO.xyy;
  barys.z = 1.0 - GPU_BARYCENTRIC_TEXCO.x - GPU_BARYCENTRIC_TEXCO.y;

  size *= 0.5;
  vec3 s = step(-size, -barys * GPU_BARYCENTRIC_DIST);

  fac = max(s.x, max(s.y, s.z));
}

void node_wireframe_screenspace(float size, vec2 GPU_BARYCENTRIC_TEXCO,vec3 GPU_BARYCENTRIC_DIST, out float fac)
{
  vec3 barys = GPU_BARYCENTRIC_TEXCO.xyy;
  barys.z = 1.0 - GPU_BARYCENTRIC_TEXCO.x - GPU_BARYCENTRIC_TEXCO.y;

  size *= (1.0 / 3.0);
  vec3 dx = dFdx(barys);
  vec3 dy = dFdy(barys);
  vec3 deltas = sqrt(dx * dx + dy * dy);

  vec3 s = step(-deltas * size, -barys);

  fac = max(s.x, max(s.y, s.z));
}



void node_node_wireframe(float size,float use_pixel_size, vec2 GPU_BARYCENTRIC_TEXCO, vec3 GPU_BARYCENTRIC_DIST, out float fac){
  if (use_pixel_size == 1.0){
    node_wireframe_screenspace(size,GPU_BARYCENTRIC_TEXCO,GPU_BARYCENTRIC_DIST,fac);
  }else{
    node_wireframe_worldspace(size,GPU_BARYCENTRIC_TEXCO,GPU_BARYCENTRIC_DIST,fac);
  }
}

#endif
