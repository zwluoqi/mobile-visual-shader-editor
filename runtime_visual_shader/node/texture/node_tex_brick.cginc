/*
* Copyright 2021-2021 Zhouwei. All rights reserved.
* License: https://github.com/zwluoqi/mobile-visual-shader-editor#license-bsd-2-clause
*/

#ifndef node_tex_brick_api
#define node_tex_brick_api

#include "../include/base_h.cginc"

/*
砖块纹理用于添加生成砖块的程序纹理。
https://docs.blender.org/manual/en/latest/render/shader_nodes/textures/brick.html
*/

vec2 calc_brick_texture(vec3 p,
                        float mortar_size,
                        float mortar_smooth,
                        float bias,
                        float brick_width,
                        float row_height,
                        float offset_amount,
                        int offset_frequency,
                        float squash_amount,
                        int squash_frequency)
{
  int bricknum, rownum;
  float offset = 0.0;
  float x, y;

  rownum = floor_to_int(p.y / row_height);

  if (offset_frequency != 0 && squash_frequency != 0) {
    brick_width *= (rownum % squash_frequency != 0) ? 1.0 : squash_amount;           /* squash */
    offset = (rownum % offset_frequency != 0) ? 0.0 : (brick_width * offset_amount); /* offset */
  }

  bricknum = floor_to_int((p.x + offset) / brick_width);

  x = (p.x + offset) - brick_width * bricknum;
  y = p.y - row_height * rownum;

  float tint = clamp((integer_noise((rownum << 16) + (bricknum & 0xFFFF)) + bias), 0.0, 1.0);

  float min_dist = min(min(x, y), min(brick_width - x, row_height - y));
  if (min_dist >= mortar_size) {
    return vec2(tint, 0.0);
  }
  else if (mortar_smooth == 0.0) {
    return vec2(tint, 1.0);
  }
  else {
    min_dist = 1.0 - min_dist / mortar_size;
    return vec2(tint, smoothstep(0.0, mortar_smooth, min_dist));
  }
}

//1.没有输入的时候,使用默认顶点的偏导数得到的新位置0 dx方向的新位置,1 dy方向的新位置
//2.node_shader_gpu_tex_mapping做 如果有贴图映射,使用贴图的matrix重新生成新的向量,

void node_tex_brick(
                    float offset_amount,
                    float offset_frequency,
                    float squash_amount,
                    float squash_frequency,
                    vec3 co,
                    vec4 color1,
                    vec4 color2,
                    vec4 mortar,
                    float scale,
                    float mortar_size,
                    float mortar_smooth,
                    float bias,
                    float brick_width,
                    float row_height,
                    out vec4 color,
                    out float fac)
{
  //tex->offset = 0.5f;
  //tex->squash = 1.0f;
  //tex->offset_freq = 2;
  //tex->squash_freq = 2;
  
  // float offset_amount= 0.5f;
  //   float offset_frequency= 1.0f;
  //   float squash_amount= 2;
  //   float squash_frequency= 2;
  
  vec2 f2 = calc_brick_texture(co * scale,
                               mortar_size,
                               mortar_smooth,
                               bias,
                               brick_width,
                               row_height,
                               offset_amount,
                               int(offset_frequency),
                               squash_amount,
                               int(squash_frequency));
  float tint = f2.x;
  float f = f2.y;
  if (f != 1.0) {
    float facm = 1.0 - tint;
    color1 = facm * color1 + tint * color2;
  }
  color = mix(color1, mortar, f);
  fac = f;
}


#endif
