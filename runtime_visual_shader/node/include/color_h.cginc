/*
* Copyright 2021-2021 Zhouwei. All rights reserved.
* License: https://github.com/zwluoqi/mobile-visual-shader-editor#license-bsd-2-clause
*/

#ifndef color_h
#define color_h
float color_srgb_to_scene_linear(float c)
{
  if (c < 0.04045)
    return (c < 0.0) ? 0.0 : c * (1.0 / 12.92);
  else
    return pow((c + 0.055) * (1.0 / 1.055), 2.4);
}

float color_scene_linear_to_srgb(float c)
{
  if (c < 0.0031308)
    return (c < 0.0) ? 0.0 : c * 12.92;
  else
    return 1.055 * pow(c, 1.0 / 2.4) - 0.055;
}

float3 color_srgb_to_scene_linear(float3 c)
{
  return float3(color_srgb_to_scene_linear(c[0]),
               color_srgb_to_scene_linear(c[1]),
               color_srgb_to_scene_linear(c[2]));
}

float3 color_scene_linear_to_srgb(float3 c)
{
  return float3(color_scene_linear_to_srgb(c[0]),
               color_scene_linear_to_srgb(c[1]),
               color_scene_linear_to_srgb(c[2]));
}

float3 color_unpremultiply(float3 c, float alpha)
{
  if (alpha != 1.0 && alpha != 0.0)
    return c / alpha;

  return c;
}

/* Color Operations */

float3 xyY_to_xyz(float x, float y, float Y)
{
  float X, Z;

  if (y != 0.0)
    X = (x / y) * Y;
  else
    X = 0.0;

  if (y != 0.0 && Y != 0.0)
    Z = ((1.0 - x - y) / y) * Y;
  else
    Z = 0.0;

  return float3(X, Y, Z);
}

float3 xyz_to_rgb(float x, float y, float z)
{
  return float3(3.240479 * x + -1.537150 * y + -0.498535 * z,
               -0.969256 * x + 1.875991 * y + 0.041556 * z,
               0.055648 * x + -0.204043 * y + 1.057311 * z);
}

float3 xyz_to_rgb(float3 rgb)
{
    return xyz_to_rgb(rgb.x,rgb.y,rgb.z);
}

void rgb_to_hsv(float4 rgb, out float4 outcol)
{
  float cmax, cmin, h, s, v, cdelta;
  float3 c;

  cmax = max(rgb[0], max(rgb[1], rgb[2]));
  cmin = min(rgb[0], min(rgb[1], rgb[2]));
  cdelta = cmax - cmin;

  v = cmax;
  if (cmax != 0.0) {
    s = cdelta / cmax;
  }
  else {
    s = 0.0;
    h = 0.0;
  }

  if (s == 0.0) {
    h = 0.0;
  }
  else {
    c = (float3(cmax,cmax,cmax) - rgb.xyz) / cdelta;

    if (rgb.x == cmax) {
      h = c[2] - c[1];
    }
    else if (rgb.y == cmax) {
      h = 2.0 + c[0] - c[2];
    }
    else {
      h = 4.0 + c[1] - c[0];
    }

    h /= 6.0;

    if (h < 0.0) {
      h += 1.0;
    }
  }

  outcol = float4(h, s, v, rgb.w);
}

void hsv_to_rgb(float4 hsv, out float4 outcol)
{
  float i, f, p, q, t, h, s, v;
  float3 rgb;

  h = hsv[0];
  s = hsv[1];
  v = hsv[2];

  if (s == 0.0) {
    rgb = float3(v, v, v);
  }
  else {
    if (h == 1.0) {
      h = 0.0;
    }

    h *= 6.0;
    i = floor(h);
    f = h - i;
    rgb = float3(f, f, f);
    p = v * (1.0 - s);
    q = v * (1.0 - (s * f));
    t = v * (1.0 - (s * (1.0 - f)));

    if (i == 0.0) {
      rgb = float3(v, t, p);
    }
    else if (i == 1.0) {
      rgb = float3(q, v, p);
    }
    else if (i == 2.0) {
      rgb = float3(p, v, t);
    }
    else if (i == 3.0) {
      rgb = float3(p, q, v);
    }
    else if (i == 4.0) {
      rgb = float3(t, p, v);
    }
    else {
      rgb = float3(v, p, q);
    }
  }

  outcol = float4(rgb, hsv.w);
}

void color_alpha_clear(float4 color, out float4 result)
{
  result = float4(color.rgb, 1.0);
}

void color_alpha_premultiply(float4 color, out float4 result)
{
  result = float4(color.rgb * color.a, 1.0);
}

void color_alpha_unpremultiply(float4 color, out float4 result)
{
  if (color.a == 0.0 || color.a == 1.0) {
    result = float4(color.rgb, 1.0);
  }
  else {
    result = float4(color.rgb / color.a, 1.0);
  }
}
#endif
