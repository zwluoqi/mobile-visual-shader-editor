/*
* Copyright 2021-2021 Zhouwei. All rights reserved.
* License: https://github.com/zwluoqi/mobile-visual-shader-editor#license-bsd-2-clause
*/


#ifndef node_mix_api
#define node_mix_api
//#include "UnityCG.cginc"
#include "../include/base_h.cginc"

void mix_blend(float use_clamp, float fac, float4 col1, float4 col2, out float4 outcol)
{
  fac = clamp(fac, 0.0, 1.0);
  outcol = mix(col1, col2, fac);
  outcol.a = col1.a;
}

void mix_add(float use_clamp, float fac, float4 col1, float4 col2, out float4 outcol)
{
  fac = clamp(fac, 0.0, 1.0);
  outcol = mix(col1, col1 + col2, fac);
  outcol.a = col1.a;
}

void mix_multiply(float use_clamp, float fac, float4 col1, float4 col2, out float4 outcol)
{
  fac = clamp(fac, 0.0, 1.0);
  outcol = mix(col1, col1 * col2, fac);
  outcol.a = col1.a;
}

void mix_screen(float use_clamp, float fac, float4 col1, float4 col2, out float4 outcol)
{
  fac = clamp(fac, 0.0, 1.0);
  float facm = 1.0 - fac;

  outcol = float4(1.0,1.0,1.0,1.0) - (float4(facm,facm,facm,facm) + fac * (float4(1.0,1.0,1.0,1.0) - col2)) * (float4(1.0,1.0,1.0,1.0) - col1);
  outcol.a = col1.a;
}

void mix_overlay(float use_clamp, float fac, float4 col1, float4 col2, out float4 outcol)
{
  fac = clamp(fac, 0.0, 1.0);
  float facm = 1.0 - fac;

  outcol = col1;

  if (outcol.r < 0.5) {
    outcol.r *= facm + 2.0 * fac * col2.r;
  }
  else {
    outcol.r = 1.0 - (facm + 2.0 * fac * (1.0 - col2.r)) * (1.0 - outcol.r);
  }

  if (outcol.g < 0.5) {
    outcol.g *= facm + 2.0 * fac * col2.g;
  }
  else {
    outcol.g = 1.0 - (facm + 2.0 * fac * (1.0 - col2.g)) * (1.0 - outcol.g);
  }

  if (outcol.b < 0.5) {
    outcol.b *= facm + 2.0 * fac * col2.b;
  }
  else {
    outcol.b = 1.0 - (facm + 2.0 * fac * (1.0 - col2.b)) * (1.0 - outcol.b);
  }
}

void mix_subtract(float use_clamp, float fac, float4 col1, float4 col2, out float4 outcol)
{
  fac = clamp(fac, 0.0, 1.0);
  outcol = mix(col1, col1 - col2, fac);
  outcol.a = col1.a;
}

void mix_divide(float use_clamp, float fac, float4 col1, float4 col2, out float4 outcol)
{
  fac = clamp(fac, 0.0, 1.0);
  float facm = 1.0 - fac;

  outcol = col1;

  if (col2.r != 0.0) {
    outcol.r = facm * outcol.r + fac * outcol.r / col2.r;
  }
  if (col2.g != 0.0) {
    outcol.g = facm * outcol.g + fac * outcol.g / col2.g;
  }
  if (col2.b != 0.0) {
    outcol.b = facm * outcol.b + fac * outcol.b / col2.b;
  }
}

void mix_difference(float use_clamp, float fac, float4 col1, float4 col2, out float4 outcol)
{
  fac = clamp(fac, 0.0, 1.0);
  outcol = mix(col1, abs(col1 - col2), fac);
  outcol.a = col1.a;
}

void mix_darken(float use_clamp, float fac, float4 col1, float4 col2, out float4 outcol)
{
  fac = clamp(fac, 0.0, 1.0);
  outcol.rgb = mix(col1.rgb, min(col1.rgb, col2.rgb), fac);
  outcol.a = col1.a;
}

void mix_lighten(float use_clamp, float fac, float4 col1, float4 col2, out float4 outcol)
{
  fac = clamp(fac, 0.0, 1.0);
  outcol.rgb = mix(col1.rgb, max(col1.rgb, col2.rgb), fac);
  outcol.a = col1.a;
}

void mix_dodge(float use_clamp, float fac, float4 col1, float4 col2, out float4 outcol)
{
  fac = clamp(fac, 0.0, 1.0);
  outcol = col1;

  if (outcol.r != 0.0) {
    float tmp = 1.0 - fac * col2.r;
    if (tmp <= 0.0) {
      outcol.r = 1.0;
    }
    else if ((tmp = outcol.r / tmp) > 1.0) {
      outcol.r = 1.0;
    }
    else {
      outcol.r = tmp;
    }
  }
  if (outcol.g != 0.0) {
    float tmp = 1.0 - fac * col2.g;
    if (tmp <= 0.0) {
      outcol.g = 1.0;
    }
    else if ((tmp = outcol.g / tmp) > 1.0) {
      outcol.g = 1.0;
    }
    else {
      outcol.g = tmp;
    }
  }
  if (outcol.b != 0.0) {
    float tmp = 1.0 - fac * col2.b;
    if (tmp <= 0.0) {
      outcol.b = 1.0;
    }
    else if ((tmp = outcol.b / tmp) > 1.0) {
      outcol.b = 1.0;
    }
    else {
      outcol.b = tmp;
    }
  }
}

void mix_burn(float use_clamp, float fac, float4 col1, float4 col2, out float4 outcol)
{
  fac = clamp(fac, 0.0, 1.0);
  float tmp, facm = 1.0 - fac;

  outcol = col1;

  tmp = facm + fac * col2.r;
  if (tmp <= 0.0) {
    outcol.r = 0.0;
  }
  else if ((tmp = (1.0 - (1.0 - outcol.r) / tmp)) < 0.0) {
    outcol.r = 0.0;
  }
  else if (tmp > 1.0) {
    outcol.r = 1.0;
  }
  else {
    outcol.r = tmp;
  }

  tmp = facm + fac * col2.g;
  if (tmp <= 0.0) {
    outcol.g = 0.0;
  }
  else if ((tmp = (1.0 - (1.0 - outcol.g) / tmp)) < 0.0) {
    outcol.g = 0.0;
  }
  else if (tmp > 1.0) {
    outcol.g = 1.0;
  }
  else {
    outcol.g = tmp;
  }

  tmp = facm + fac * col2.b;
  if (tmp <= 0.0) {
    outcol.b = 0.0;
  }
  else if ((tmp = (1.0 - (1.0 - outcol.b) / tmp)) < 0.0) {
    outcol.b = 0.0;
  }
  else if (tmp > 1.0) {
    outcol.b = 1.0;
  }
  else {
    outcol.b = tmp;
  }
}

void mix_hue(float use_clamp, float fac, float4 col1, float4 col2, out float4 outcol)
{
  fac = clamp(fac, 0.0, 1.0);
  float facm = 1.0 - fac;

  outcol = col1;

  float4 hsv, hsv2, tmp;
  rgb_to_hsv(col2, hsv2);

  if (hsv2.y != 0.0) {
    rgb_to_hsv(outcol, hsv);
    hsv.x = hsv2.x;
    hsv_to_rgb(hsv, tmp);

    outcol = mix(outcol, tmp, fac);
    outcol.a = col1.a;
  }
}

void mix_saturation(float use_clamp, float fac, float4 col1, float4 col2, out float4 outcol)
{
  fac = clamp(fac, 0.0, 1.0);
  float facm = 1.0 - fac;

  outcol = col1;

  float4 hsv, hsv2;
  rgb_to_hsv(outcol, hsv);

  if (hsv.y != 0.0) {
    rgb_to_hsv(col2, hsv2);

    hsv.y = facm * hsv.y + fac * hsv2.y;
    hsv_to_rgb(hsv, outcol);
  }
}

void mix_value(float use_clamp, float fac, float4 col1, float4 col2, out float4 outcol)
{
  fac = clamp(fac, 0.0, 1.0);
  float facm = 1.0 - fac;

  float4 hsv, hsv2;
  rgb_to_hsv(col1, hsv);
  rgb_to_hsv(col2, hsv2);

  hsv.z = facm * hsv.z + fac * hsv2.z;
  hsv_to_rgb(hsv, outcol);
}

void mix_color(float use_clamp, float fac, float4 col1, float4 col2, out float4 outcol)
{
  fac = clamp(fac, 0.0, 1.0);
  float facm = 1.0 - fac;

  outcol = col1;

  float4 hsv, hsv2, tmp;
  rgb_to_hsv(col2, hsv2);

  if (hsv2.y != 0.0) {
    rgb_to_hsv(outcol, hsv);
    hsv.x = hsv2.x;
    hsv.y = hsv2.y;
    hsv_to_rgb(hsv, tmp);

    outcol = mix(outcol, tmp, fac);
    outcol.a = col1.a;
  }
}

void mix_soft_light(float use_clamp, float fac, float4 col1, float4 col2, out float4 outcol)
{
  fac = clamp(fac, 0.0, 1.0);
  float facm = 1.0 - fac;

  float4 one = float4(1.0,1.0,1.0,1.0);
  float4 scr = one - (one - col2) * (one - col1);
  outcol = facm * col1 + fac * ((one - col1) * col2 * col1 + col1 * scr);
}

void mix_linear_light(float use_clamp, float fac, float4 col1, float4 col2, out float4 outcol)
{
  fac = clamp(fac, 0.0, 1.0);

  outcol = col1 + fac * (2.0 * (col2 - float4(0.5,0.5,0.5,0.5)));
}

void clamp_color(float use_clamp, float3 vec, float3 min, float3 max, out float3 out_vec)
{
  out_vec = clamp(vec, min, max);
}


#endif
