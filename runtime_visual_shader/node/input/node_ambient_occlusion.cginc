/*
* Copyright 2021-2021 Zhouwei. All rights reserved.
* License: https://github.com/zwluoqi/mobile-visual-shader-editor#license-bsd-2-clause
*/


#ifndef node_ambient_occlusion_api
#define node_ambient_occlusion_api
//#include "UnityCG.cginc"
#include "../include/base_h.cginc"
/*
需要运行时g-buffer支持,需要screenspaceraytracy,暂时不支持改节点
*/
void node_ambient_occlusion(
                            float sample_count,
                            float inside,
                            float only_local,
                            
                            vec4 color,
                            float dist,
                            vec3 GPU_WORLD_NORMAL,//default
                            vec3 GPU_WORLD_POSITION ,
                            vec3 GPU_FragCoord,
                            out vec4 result_color,
                            out float result_ao)
{
  vec3 bent_normal;
  vec4 rand = texelfetch_noise_tex(GPU_FragCoord.xy);
  //需要g-buffer支持,
  //OcclusionData data = occlusion_search(viewPosition, maxzBuffer, dist, inverted, sample_count);

  vec3 V = cameraVec(GPU_WORLD_POSITION);
  vec3 N = normalize(GPU_WORLD_NORMAL);
  vec3 Ng = safe_normalize(cross(dFdx(GPU_WORLD_POSITION), dFdy(GPU_WORLD_POSITION)));

  float unused_error;
  vec3 unused;
  occlusion_eval(data, V, N, Ng, inside, result_ao, unused_error, unused);
  result_color = result_ao * color;
}

#endif
