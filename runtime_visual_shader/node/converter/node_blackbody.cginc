/*
* Copyright 2021-2021 Zhouwei. All rights reserved.
* License: https://github.com/zwluoqi/mobile-visual-shader-editor#license-bsd-2-clause
*/


#ifndef node_blackbody_api
#define node_blackbody_api
//#include "UnityCG.cginc"
#include "../include/base_h.cginc"

//https://docs.unity3d.com/Packages/com.unity.shadergraph@8.0/manual/Blackbody-Node.html
//https://github.com/glslify/glsl-colormap/blob/master/readme.md
//= 1200.0
void node_blackbody(float Temperature , out float4 col)
{  
    float3 color = float3(255.0, 255.0, 255.0);
    color.x = 56100000. * pow(Temperature,(-3.0 / 2.0)) + 148.0;
    color.y = 100.04 * log(Temperature) - 623.6;
    if (Temperature > 6500.0) {color.y = 35200000.0 * pow(Temperature,(-3.0 / 2.0)) + 184.0;}
    color.z = 194.18 * log(Temperature) - 1448.6;
    color = clamp(color, 0.0, 255.0)/255.0;
    if (Temperature < 1000.0) {color *= Temperature/1000.0;}

    col = float4(color,1.0);
}

#endif
