/*
* Copyright 2021-2021 Zhouwei. All rights reserved.
* License: https://github.com/zwluoqi/mobile-visual-shader-editor#license-bsd-2-clause
*/


#ifndef node_combine_xyz_api
#define node_combine_xyz_api
//#include "UnityCG.cginc"
#include "../include/base_h.cginc"

void node_combine_xyz(float x, float y, float z, out float4 vec)
{
  vec = float4(x, y, z,1.0);
}


#endif
