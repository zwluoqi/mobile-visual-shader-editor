/*
* Copyright 2021-2021 Zhouwei. All rights reserved.
* License: https://github.com/zwluoqi/mobile-visual-shader-editor#license-bsd-2-clause
*/

#ifndef node_gamma_api
#define node_gamma_api
#include "../include/base_h.cginc"

void node_gamma(float4 ColorIn , float Gamma ,out float4 ColorOut)
{
  ColorOut = pow(ColorIn, Gamma);
}

#endif
