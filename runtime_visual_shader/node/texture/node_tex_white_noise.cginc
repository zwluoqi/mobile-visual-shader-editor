/*
* Copyright 2021-2021 Zhouwei. All rights reserved.
* License: https://github.com/zwluoqi/mobile-visual-shader-editor#license-bsd-2-clause
*/

#ifndef node_white_noise_api
#define node_white_noise_api

#include "../include/base_h.cginc"

/*
白噪声纹理节点根据输入的Seed返回一个随机数。种子可以是一个数字，一个2D矢量，一个3D矢量，或者一个4D矢量;取决于Dimensions属性。输出数字的范围在0到1之间。
https://docs.blender.org/manual/en/latest/render/shader_nodes/textures/white_noise.html
*/
/* White Noise */

void node_white_noise_1d(vec3 v, float w, out float value, out vec4 color)
{
  value = hash_float_to_float(w);
  color.xyz = hash_float_to_vec3(w);
}

void node_white_noise_2d(vec3 v, float w, out float value, out vec4 color)
{
  value = hash_vec2_to_float(v.xy);
  color.xyz = hash_vec2_to_vec3(v.xy);
}

void node_white_noise_3d(vec3 v, float w, out float value, out vec4 color)
{
  value = hash_vec3_to_float(v);
  color.xyz = hash_vec3_to_vec3(v);
}

void node_white_noise_4d(vec3 v, float w, out float value, out vec4 color)
{
  value = hash_vec4_to_float(vec4(v, w));
  color.xyz = hash_vec4_to_vec3(vec4(v, w));
}


#endif
