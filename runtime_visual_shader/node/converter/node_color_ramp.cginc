/*
* Copyright 2021-2021 Zhouwei. All rights reserved.
* License: https://github.com/zwluoqi/mobile-visual-shader-editor#license-bsd-2-clause
*/

#ifndef node_color_ramp_h
#define node_color_ramp_h


/*
https://docs.blender.org/manual/en/latest/render/shader_nodes/converter/color_ramp.html
颜色渐变节点用于使用渐变将值映射到颜色。
*/

/*
不需要的功能,直接走下面的整合函数即可
void valtorgb_opti_constant(
    float fac, float edge, vec4 color1, vec4 color2, out vec4 outcol, out float outalpha)
{
  outcol = (fac > edge) ? color2 : color1;
  outalpha = outcol.a;
}

void valtorgb_opti_linear(
    float fac, vec2 mulbias, vec4 color1, vec4 color2, out vec4 outcol, out float outalpha)
{
  fac = clamp(fac * mulbias.x + mulbias.y, 0.0, 1.0);
  outcol = mix(color1, color2, fac);
  outalpha = outcol.a;
}

void valtorgb_opti_ease(
    float fac, vec2 mulbias, vec4 color1, vec4 color2, out vec4 outcol, out float outalpha)
{
  fac = clamp(fac * mulbias.x + mulbias.y, 0.0, 1.0);
  fac = fac * fac * (3.0 - 2.0 * fac);
  outcol = mix(color1, color2, fac);
  outalpha = outcol.a;
}
*/

void valtorgb(float fac, sampler1DArray colormap, float layer, out vec4 outcol, out float outalpha)
{
  outcol = texture2D(colormap, vec2(fac, layer));
  outalpha = outcol.a;
}

void valtorgb_nearest(
    float fac, sampler1DArray colormap, vec4 colormapuv, float layer, out vec4 outcol, out float outalpha)
{
  fac = clamp(fac, 0.0, 1.0);
  outcol = texelFetch(colormap, vec2(fac * (colormapuv.x - 1), layer),0.0);
  outalpha = outcol.a;
}

#endif
