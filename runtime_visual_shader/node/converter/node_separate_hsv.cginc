/*
* Copyright 2021-2021 Zhouwei. All rights reserved.
* License: https://github.com/zwluoqi/mobile-visual-shader-editor#license-bsd-2-clause
*/


#ifndef node_separate_hsv_api
#define node_separate_hsv_api
//#include "UnityCG.cginc"
#include "../include/base_h.cginc"

void node_separate_hsv(float4 col, out float h, out float s, out float v)
{
  float4 hsv;

  rgb_to_hsv(col, hsv);
  h = hsv[0];
  s = hsv[1];
  v = hsv[2];
}


#endif
