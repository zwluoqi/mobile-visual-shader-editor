/*
* Copyright 2021-2021 Zhouwei. All rights reserved.
* License: https://github.com/zwluoqi/mobile-visual-shader-editor#license-bsd-2-clause
*/

#ifndef light_h
#define light_h
#include "define_h.cginc"




uniform sampler3D utilTex;



#define LUT_SIZE 64

#define LTC_MAT_LAYER 0
#define LTC_BRDF_LAYER 1
#define BRDF_LUT_LAYER 1
#define NOISE_LAYER 2
#define LTC_DISK_LAYER 3 /* UNUSED */

/**
 * Reminder: The 4 noise values are based of 3 uncorrelated blue noises:
 * x : Uniformly distributed value [0..1] (noise 1).
 * y : Uniformly distributed value [0..1] (noise 2).
 * z,w : Uniformly distributed point on the unit circle [-1..1] (noise 3).
 */
#define texelfetch_noise_tex(coord) texelFetch3D(utilTex, vec3(vec2(coord) % LUT_SIZE,2.0),0.0)


#endif
