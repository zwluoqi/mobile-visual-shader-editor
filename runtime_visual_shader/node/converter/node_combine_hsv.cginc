/*
* Copyright 2021-2021 Zhouwei. All rights reserved.
* License: https://github.com/zwluoqi/mobile-visual-shader-editor#license-bsd-2-clause
*/


#ifndef node_combine_hsv_api
#define node_combine_hsv_api
//#include "UnityCG.cginc"
#include "../include/base_h.cginc"

void node_combine_hsv(float h, float s, float v, out float4 col)
{
  hsv_to_rgb(float4(h, s, v, 1.0), col);
}


#endif
