/*
* Copyright 2021-2021 Zhouwei. All rights reserved.
* License: https://github.com/zwluoqi/mobile-visual-shader-editor#license-bsd-2-clause
*/

#ifndef node_bsdf_emission_h
#define node_bsdf_emission_h

#include "../include/base_h.cginc"
#include "../light/closure_eval_lib.cginc"


void node_bsdf_emission(vec4 color, float strength, out Closure result){
      result.radiance = uLightRadiance*color*strength;
}

#endif
