/*
* Copyright 2021-2021 Zhouwei. All rights reserved.
* License: https://github.com/zwluoqi/mobile-visual-shader-editor#license-bsd-2-clause
*/


#ifndef node_rgb_api
#define node_rgb_api
//#include "UnityCG.cginc"
#include "../include/base_h.cginc"
/*
GPU_OBJECT_INFO?
*/
void node_rgb(vec3 invec,inout vec4 color)
{
    color = vec4(invec,1.0);
}



#endif
