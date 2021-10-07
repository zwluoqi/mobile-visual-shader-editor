/*
* Copyright 2021-2021 Zhouwei. All rights reserved.
* License: https://github.com/zwluoqi/mobile-visual-shader-editor#license-bsd-2-clause
*/

#ifndef node_add_shader_h
#define node_add_shader_h

#include "../include/base_h.cginc"
#include "../light/closure_eval_lib.cginc"

void node_add_shader(Closure shader1, Closure shader2, out Closure shader)
{
  shader = closure_add(shader1, shader2);
}
#endif
