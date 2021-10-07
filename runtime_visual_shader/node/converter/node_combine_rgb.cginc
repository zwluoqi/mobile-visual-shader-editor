/*
* Copyright 2021-2021 Zhouwei. All rights reserved.
* License: https://github.com/zwluoqi/mobile-visual-shader-editor#license-bsd-2-clause
*/


#ifndef node_combine_rgb_api
#define node_combine_rgb_api
//#include "UnityCG.cginc"
#include "../include/base_h.cginc"

void node_combine_rgb(float r, float g, float b, out float4 col)
{
  col = float4(r, g, b, 1.0);
}


#endif
