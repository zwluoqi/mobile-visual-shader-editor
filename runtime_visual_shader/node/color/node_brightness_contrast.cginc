/*
* Copyright 2021-2021 Zhouwei. All rights reserved.
* License: https://github.com/zwluoqi/mobile-visual-shader-editor#license-bsd-2-clause
*/

#ifndef node_brightness_contrast_api
#define node_brightness_contrast_api
#include "../include/base_h.cginc"

/*
颜色校正Color Adjustments
亮度(brightness)
明暗对比(contrast)
*/
void node_brightness_contrast(float4 col, float brightness, float contrast, out float4 outcol)
{
  float a = 1.0 + contrast;
  float b = brightness - contrast * 0.5;

  outcol.r = max(a * col.r + b, 0.0);
  outcol.g = max(a * col.g + b, 0.0);
  outcol.b = max(a * col.b + b, 0.0);
  outcol.a = col.a;
}

#endif
