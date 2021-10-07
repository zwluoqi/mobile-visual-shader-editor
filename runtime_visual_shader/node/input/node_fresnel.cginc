/*
* Copyright 2021-2021 Zhouwei. All rights reserved.
* License: https://github.com/zwluoqi/mobile-visual-shader-editor#license-bsd-2-clause
*/


#ifndef node_fresnel_api
#define node_fresnel_api
//#include "UnityCG.cginc"
#include "../include/base_h.cginc"
/*

*/
float fresnel_dielectric_cos(float cosi, float eta)
{
  /* compute fresnel reflectance without explicitly computing
   * the refracted direction */
  float c = abs(cosi);
  float g = eta * eta - 1.0 + c * c;
  float result;

  if (g > 0.0) {
    g = sqrt(g);
    float A = (g - c) / (g + c);
    float B = (c * (g + c) - 1.0) / (c * (g - c) + 1.0);
    result = 0.5 * A * A * (1.0 + B * B);
  }
  else {
    result = 1.0; /* TIR (no refracted component) */
  }

  return result;
}

float fresnel_dielectric(vec3 Incoming, vec3 Normal, float eta)
{
  /* compute fresnel reflectance without explicitly computing
   * the refracted direction */
  return fresnel_dielectric_cos(dot(Incoming, Normal), eta);
}

//viewDir fresnel反射结果
void node_fresnel(float ior, vec3 GPU_WORLD_NORMAL,vec3 GPU_VIEW_POSITION,  out float result)
{
  //vec3 I = viewDir.xyz;
  vec3 viewNormal;
  direction_transform_m4v3(GPU_WORLD_NORMAL,viewmat,viewNormal);
  viewNormal = normalize(viewNormal);
  /* handle perspective/orthographic */
  vec3 I_view = (u_proj[3][3] == 0.0) ? normalize(GPU_VIEW_POSITION) : vec3(0.0, 0.0, -1.0);

  float eta = max(ior, 0.00001);
  result = fresnel_dielectric(I_view, viewNormal, eta);
}

#endif
