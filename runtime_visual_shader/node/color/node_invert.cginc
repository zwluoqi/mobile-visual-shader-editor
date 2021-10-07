/*
* Copyright 2021-2021 Zhouwei. All rights reserved.
* License: https://github.com/zwluoqi/mobile-visual-shader-editor#license-bsd-2-clause
*/


#ifndef node_invert_api
#define node_invert_api
//#include "UnityCG.cginc"
#include "../include/base_h.cginc"

void node_invert(float Fac , float4 ColorIn ,
                out float4 ColorOut)
{
  float4 ColorInv = 1.0 - ColorIn;
  ColorOut = lerp(ColorIn, ColorInv, Fac); 
}
#endif
