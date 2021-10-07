/*
* Copyright 2021-2021 Zhouwei. All rights reserved.
* License: https://github.com/zwluoqi/mobile-visual-shader-editor#license-bsd-2-clause
*/


#ifndef node_separate_xyz_api
#define node_separate_xyz_api
//#include "UnityCG.cginc"
#include "../include/base_h.cginc"

void node_separate_xyz(float3 vec, out float x, out float y, out float z)
{
  x = vec.r;
  y = vec.g;
  z = vec.b;
}


#endif
