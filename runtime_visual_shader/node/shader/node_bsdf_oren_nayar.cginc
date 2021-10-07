/*
* Copyright 2021-2021 Zhouwei. All rights reserved.
* License: https://github.com/zwluoqi/mobile-visual-shader-editor#license-bsd-2-clause
*/

#ifndef node_bsdf_oren_nayar_h
#define node_bsdf_oren_nayar_h

#include "../include/base_h.cginc"
#include "../light/closure_eval_lib.cginc"

void node_bsdf_orenNayar(vec4 color, float roughness, vec3 GPU_WORLD_NORMAL,vec3 GPU_WORLD_POSITION, out Closure result){
      vec3 lightDirection = normalize(uLightDir.xyz);
      vec3 viewDirection = cameraVec(GPU_WORLD_POSITION);
      vec3 surfaceNormal = GPU_WORLD_NORMAL;

      float LdotV = dot(lightDirection, viewDirection);
      float NdotL = dot(lightDirection, surfaceNormal);
      float NdotV = dot(surfaceNormal, viewDirection);
  
      float ga = dot(viewDirection-surfaceNormal*NdotV,lightDirection-surfaceNormal*NdotL);
      float alpha = max(NdotL,NdotV);
      float beta = min(NdotL,NdotV);
      
      
  
      float sigma2 = roughness * roughness;
      float A = 1.0 -  0.5*sigma2 / (sigma2 + 0.33);
      float B = 0.45 * sigma2 / (sigma2 + 0.09);
  
      float pdf =  max(0.0, NdotL) * (A + B * max(0,ga)*sin(alpha)*tan(beta) ) / PI;

      result.radiance = pdf*uLightRadiance*color;
}

#endif