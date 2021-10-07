/*
* Copyright 2021-2021 Zhouwei. All rights reserved.
* License: https://github.com/zwluoqi/mobile-visual-shader-editor#license-bsd-2-clause
*/


#ifndef node_vertex_color_api
#define node_vertex_color_api
//#include "UnityCG.cginc"
#include "../include/base_h.cginc"

void node_vertex_color(float4 invalue,out float3 ret,out float alpha)
{
    ret = invalue.xyz;
    alpha = invalue.w;
}

#endif
