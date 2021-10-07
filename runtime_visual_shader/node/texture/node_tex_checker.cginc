/*
* Copyright 2021-2021 Zhouwei. All rights reserved.
* License: https://github.com/zwluoqi/mobile-visual-shader-editor#license-bsd-2-clause
*/

#ifndef node_tex_checker_api
#define node_tex_checker_api

#include "../include/base_h.cginc"

/*
用于添加一个棋盘纹理,
Vector,如果未连接，默认为纹理坐标
https://docs.blender.org/manual/en/latest/render/shader_nodes/textures/checker.html
*/

//1.没有输入的时候,使用默认顶点的偏导数得到的新位置0 dx方向的新位置,1 dy方向的新位置
//2.node_shader_gpu_tex_mapping做 如果有贴图映射,使用贴图的matrix重新生成新的向量,

void node_tex_checker(
    float3 co, float4 color1, float4 color2, float scale, out float4 color, out float fac)
{
  float3 p = co * scale;

  /* Prevent precision issues on unit coordinates. */
  p = (p + 0.000001) * 0.999999;

  int xi = int(abs(floor(p.x)));
  int yi = int(abs(floor(p.y)));
  int zi = int(abs(floor(p.z)));

  bool check = ((mod(xi, 2) == mod(yi, 2)) == bool(mod(zi, 2)));

  color = check ? color1 : color2;
  fac = check ? 1.0 : 0.0;
}

#endif
