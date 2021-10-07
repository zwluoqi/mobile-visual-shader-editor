/*
* Copyright 2021-2021 Zhouwei. All rights reserved.
* License: https://github.com/zwluoqi/mobile-visual-shader-editor#license-bsd-2-clause
*/


#ifndef node_normal_api
#define node_normal_api
//#include "UnityCG.cginc"
#include "../include/base_h.cginc"

/*

https://docs.blender.org/manual/en/latest/render/shader_nodes/vector/normal.html
法向量节点生成一个法向量和一个点积。
*/

void node_normal(vec3 nor, vec3 dir, out vec3 outnor, out float outdot)
{
  outnor = dir;
  outdot = dot(normalize(nor), dir);
}


#endif
