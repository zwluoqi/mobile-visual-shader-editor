/*
* Copyright 2021-2021 Zhouwei. All rights reserved.
* License: https://github.com/zwluoqi/mobile-visual-shader-editor#license-bsd-2-clause
*/


#ifndef node_separate_rgb_api
#define node_separate_rgb_api
//#include "UnityCG.cginc"
#include "../include/base_h.cginc"

void node_separate_rgb(float4 col, out float r, out float g, out float b)
{
  r = col.r;
  g = col.g;
  b = col.b;
}


#endif
