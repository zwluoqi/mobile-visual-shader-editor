/*
* Copyright 2021-2021 Zhouwei. All rights reserved.
* License: https://github.com/zwluoqi/mobile-visual-shader-editor#license-bsd-2-clause
*/


#ifndef node_mapping_api
#define node_mapping_api
//#include "UnityCG.cginc"
#include "../include/base_h.cginc"

/*
https://docs.blender.org/manual/en/latest/render/shader_nodes/vector/mapping.html
Mapping节点通过应用平移、旋转和缩放来转换输入向量。
先缩放、再旋转、再平移
*/
void mapping_mat4(
    vec3 vec, vec4 m0, vec4 m1, vec4 m2, vec4 m3, vec3 minvec, vec3 maxvec, out vec3 outvec)
{
  mat4 mat = mat4(m0, m1, m2, m3);
  outvec = mul(mat , vec4(vec, 1.0)).xyz;
  outvec = clamp(outvec, minvec, maxvec);
}

void mapping_point(vec3 pos, vec3 location, vec3 rotation, vec3 scale, out vec3 result)
{
  result = mul(euler_to_mat3(rotation) , (pos * scale)) + location;
}

void mapping_texture(vec3 dir, vec3 location, vec3 rotation, vec3 scale, out vec3 result)
{
  result = safe_divide(mul(transpose(euler_to_mat3(rotation)) , (dir - location)), scale);
}

void mapping_vector(vec3 dir, vec3 location, vec3 rotation, vec3 scale, out vec3 result)
{
  result = mul(euler_to_mat3(rotation) , (dir * scale));
}

void mapping_normal(vec3 nor, vec3 location, vec3 rotation, vec3 scale, out vec3 result)
{
  result = normalize( mul(euler_to_mat3(rotation) , safe_divide(nor, scale)));
}

#endif
