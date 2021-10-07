/*
* Copyright 2021-2021 Zhouwei. All rights reserved.
* License: https://github.com/zwluoqi/mobile-visual-shader-editor#license-bsd-2-clause
*/


#ifndef node_layer_weight_api
#define node_layer_weight_api
//#include "UnityCG.cginc"
#include "../include/base_h.cginc"
/*
Layer Weight节点输出一个通常用于混合着色节点的分层着色器的权重。
https://docs.blender.org/manual/en/latest/render/shader_nodes/input/layer_weight.html
*/
void node_layer_weight(float blend, vec3 GPU_WORLD_NORMAL, vec3 GPU_VIEW_POSITION, out float fresnel, out float facing)
{

  vec3 N_view;
  direction_transform_m4v3(GPU_WORLD_NORMAL,viewmat,N_view);
  N_view = normalize(N_view);

  /* fresnel */
  float eta = max(1.0 - blend, 0.00001);
  vec3 I_view = (ProjectionMatrix[3][3] == 0.0) ? normalize(GPU_VIEW_POSITION) : vec3(0.0, 0.0, -1.0);

  fresnel = fresnel_dielectric(I_view, N_view,1.0 / eta);

  /* facing */
  facing = abs(dot(I_view, N_view));
  if (blend != 0.5) {
    blend = clamp(blend, 0.0, 0.99999);
    blend = (blend < 0.5) ? 2.0 * blend : 0.5 / (1.0 - blend);
    facing = pow(facing, blend);
  }
  facing = 1.0 - facing;
}

#endif
