/*
* Copyright 2021-2021 Zhouwei. All rights reserved.
* License: https://github.com/zwluoqi/mobile-visual-shader-editor#license-bsd-2-clause
*/

#ifndef node_tex_magic_api
#define node_tex_magic_api

#include "../include/base_h.cginc"

/*
Magic Texture节点用于添加迷幻色彩纹理。
https://docs.blender.org/manual/en/latest/render/shader_nodes/textures/magic.html
*/

//1.没有输入的时候,使用默认顶点的偏导数得到的新位置0 dx方向的新位置,1 dy方向的新位置
//2.node_shader_gpu_tex_mapping做 如果有贴图映射(blender使用单位矩阵),使用贴图的matrix重新生成新的向量,

void node_tex_magic(
      float depth, 
      vec3 co, 
      float scale,
      float distortion,
      out vec4 color, 
      out float fac)
{
  vec3 p = co * scale;
  float x = sin((p.x + p.y + p.z) * 5.0);
  float y = cos((-p.x + p.y - p.z) * 5.0);
  float z = -cos((-p.x - p.y + p.z) * 5.0);

  if (depth > 0) {
    x *= distortion;
    y *= distortion;
    z *= distortion;
    y = -cos(x - y + z);
    y *= distortion;
    if (depth > 1) {
      x = cos(x - y - z);
      x *= distortion;
      if (depth > 2) {
        z = sin(-x - y - z);
        z *= distortion;
        if (depth > 3) {
          x = -cos(-x + y - z);
          x *= distortion;
          if (depth > 4) {
            y = -sin(-x + y + z);
            y *= distortion;
            if (depth > 5) {
              y = -cos(-x + y + z);
              y *= distortion;
              if (depth > 6) {
                x = cos(x + y + z);
                x *= distortion;
                if (depth > 7) {
                  z = sin(x + y - z);
                  z *= distortion;
                  if (depth > 8) {
                    x = -cos(-x - y + z);
                    x *= distortion;
                    if (depth > 9) {
                      y = -sin(x - y + z);
                      y *= distortion;
                    }
                  }
                }
              }
            }
          }
        }
      }
    }
  }
  if (distortion != 0.0) {
    distortion *= 2.0;
    x /= distortion;
    y /= distortion;
    z /= distortion;
  }

  color = vec4(0.5 - x, 0.5 - y, 0.5 - z, 1.0);
  fac = (color.x + color.y + color.z) / 3.0;
}


#endif
