/*
* Copyright 2021-2021 Zhouwei. All rights reserved.
* License: https://github.com/zwluoqi/mobile-visual-shader-editor#license-bsd-2-clause
*/


#ifndef node_bevel_api
#define node_bevel_api
//#include "UnityCG.cginc"
#include "../include/base_h.cginc"
/*

*/
void node_bevel(float samples,float radius, vec3 GPU_WORLD_NORMAL, out vec3 result)
{
  result = GPU_WORLD_NORMAL;
}

#endif
