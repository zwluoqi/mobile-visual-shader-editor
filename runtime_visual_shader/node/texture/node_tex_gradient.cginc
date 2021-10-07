/*
* Copyright 2021-2021 Zhouwei. All rights reserved.
* License: https://github.com/zwluoqi/mobile-visual-shader-editor#license-bsd-2-clause
*/

#ifndef node_tex_gradient_api
#define node_tex_gradient_api

#include "../include/base_h.cginc"

/*
梯度纹理节点根据输入向量生成插值的颜色和强度值。
https://docs.blender.org/manual/en/latest/render/shader_nodes/textures/gradient.html
*/

float calc_gradient(vec3 p, int gradient_type)
{
  float x, y, z;
  x = p.x;
  y = p.y;
  z = p.z;
  if (gradient_type == 0) { /* linear */
    return x;
  }
  else if (gradient_type == 1) { /* quadratic */
    float r = max(x, 0.0);
    return r * r;
  }
  else if (gradient_type == 2) { /* easing */
    float r = min(max(x, 0.0), 1.0);
    float t = r * r;
    return (3.0 * t - 2.0 * t * r);
  }
  else if (gradient_type == 3) { /* diagonal */
    return (x + y) * 0.5;
  }
  else if (gradient_type == 4) { /* radial */
    return atan(y/ x) / (M_PI * 2) + 0.5;
  }
  else {
    /* Bias a little bit for the case where p is a unit length vector,
     * to get exactly zero instead of a small random value depending
     * on float precision. */
    float r = max(0.999999 - sqrt(x * x + y * y + z * z), 0.0);
    if (gradient_type == 5) { /* quadratic sphere */
      return r * r;
    }
    else if (gradient_type == 6) { /* sphere */
      return r;
    }
  }
  return 0.0;
}


//1.没有输入的时候,使用默认顶点的偏导数得到的新位置0 dx方向的新位置,1 dy方向的新位置
//2.node_shader_gpu_tex_mapping做 如果有贴图映射,使用贴图的matrix重新生成新的向量,

void node_tex_gradient(vec3 co, float gradient_type, out vec4 color, out float fac)
{
  float f = calc_gradient(co, int(gradient_type));
  f = clamp(f, 0.0, 1.0);

  color = vec4(f, f, f, 1.0);
  fac = f;
}

void node_tex_gradient_linear(vec3 co,out vec4 color, out float fac)
{
  float f = co.x;

  f = clamp(f, 0.0, 1.0);

  color = vec4(f, f, f, 1.0);
  fac = f;

}

void node_tex_gradient_quadratic(vec3 co,out vec4 color, out float fac)
{
  float r = max(co.x, 0.0);
  float f= r * r;

  f = clamp(f, 0.0, 1.0);

  color = vec4(f, f, f, 1.0);
  fac = f;
}
void node_tex_gradient_easing(vec3 co,out vec4 color, out float fac)
{

  float r = min(max(co.x, 0.0), 1.0);
  float t = r * r;
  float f = (3.0 * t - 2.0 * t * r);

  f = clamp(f, 0.0, 1.0);

  color = vec4(f, f, f, 1.0);
  fac = f;
}
void node_tex_gradient_diagonal(vec3 co,out vec4 color, out float fac)
{
  
  float f =  (co.x + co.y) * 0.5;
  
  f = clamp(f, 0.0, 1.0);

  color = vec4(f, f, f, 1.0);
  fac = f;
}
void node_tex_gradient_radial(vec3 co,out vec4 color, out float fac)
{
  float f = atan(co.y/ co.x) / (M_PI * 2) + 0.5;

    
  f = clamp(f, 0.0, 1.0);

  color = vec4(f, f, f, 1.0);
  fac = f;

}
void node_tex_gradient_quadratic_sphere(vec3 co,out vec4 color, out float fac)
{
    float r = max(0.999999 - sqrt(co.x * co.x + co.y * co.y + co.z * co.z), 0.0);
    float f = r * r;

    f = clamp(f, 0.0, 1.0);

    color = vec4(f, f, f, 1.0);
    fac = f;
}

void node_tex_gradient_spherical(vec3 co,out vec4 color, out float fac)
{
    float r = max(0.999999 - sqrt(co.x * co.x + co.y * co.y + co.z * co.z), 0.0);
    float f = r;

    f = clamp(f, 0.0, 1.0);

    color = vec4(f, f, f, 1.0);
    fac = f;
}


#endif
