/*
* Copyright 2021-2021 Zhouwei. All rights reserved.
* License: https://github.com/zwluoqi/mobile-visual-shader-editor#license-bsd-2-clause
*/


#ifndef node_hsv_api
#define node_hsv_api
//#include "UnityCG.cginc"
#include "../include/base_h.cginc"

void node_hsv(float Hue ,
                float Saturation ,
                float Value ,
                float Fac ,
                float4 ColorIn ,
                out float4 ColorOut)
{
  float4 Color;
  rgb_to_hsv(ColorIn,Color);

  // remember: fmod doesn't work for negative numbers
  Color[0] = mod(Color[0] + Hue + 0.5, 1.0);
  Color[1] = clamp(Color[1] * Saturation, 0.0, 1.0);
  Color[2] *= Value;

  hsv_to_rgb(Color,Color);

  // Clamp color to prevent negative values cauzed by oversaturation.
  Color[0] = max(Color[0], 0.0);
  Color[1] = max(Color[1], 0.0);
  Color[2] = max(Color[2], 0.0);

  ColorOut = lerp(ColorIn, Color, Fac);  
}
#endif
