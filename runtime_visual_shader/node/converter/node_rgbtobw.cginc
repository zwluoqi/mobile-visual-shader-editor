/*
* Copyright 2021-2021 Zhouwei. All rights reserved.
* License: https://github.com/zwluoqi/mobile-visual-shader-editor#license-bsd-2-clause
*/


#ifndef node_rgbtobw_api
#define node_rgbtobw_api
//#include "UnityCG.cginc"
#include "../include/base_h.cginc"

/*
https://www.rapidtables.com/convert/image/rgb-to-grayscale.html
颜色转换为灰度
*/
void node_rgbtobw(float4 color, out float outval)
{
  vec3 factors = vec3(0.2126, 0.7152, 0.0722);
  outval = dot(color.rgb, factors);
}


#endif
