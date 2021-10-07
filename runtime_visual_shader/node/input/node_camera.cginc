/*
* Copyright 2021-2021 Zhouwei. All rights reserved.
* License: https://github.com/zwluoqi/mobile-visual-shader-editor#license-bsd-2-clause
*/


#ifndef node_camera_api
#define node_camera_api
//#include "UnityCG.cginc"
#include "../include/base_h.cginc"
/*
viewDir 默认世界空间视方向 隐藏节点
*/
void node_camera(vec3 GPU_WORLD_POSITION , out float4 ViewVector ,
                   out float ViewZDepth ,
                   out float ViewDistance)
{
  vec3 viewDir = cameraVec(GPU_WORLD_POSITION);
  ViewVector.xyz = viewDir.xyz;

  ViewZDepth = abs(viewDir[2]);
  ViewDistance = length(viewDir);

  ViewVector.xyz = normalize(viewDir);
}

#endif
