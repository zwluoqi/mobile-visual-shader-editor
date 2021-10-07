/*
* Copyright 2021-2021 Zhouwei. All rights reserved.
* License: https://github.com/zwluoqi/mobile-visual-shader-editor#license-bsd-2-clause
*/


#ifndef node_curves_rgb_api
#define node_curves_rgb_api
//#include "UnityCG.cginc"
#include "../include/base_h.cginc"

/* ext is vec4(in_x, in_dy, out_x, out_dy). */
float curve_extrapolate(float x, float y, vec4 ext)
{
  if (x < 0.0) {
    return y + x * ext.y;
  }
  else if (x > 1.0) {
    return y + (x - 1.0) * ext.w;
  }
  else {
    return y;
  }
}

#define RANGE_RESCALE(x, min, range) ((x - min) * range)

void curves_rgb(float layer,
                float fac,
                vec4 col,
                vec4 range,
                vec4 ext_r,
                vec4 ext_g,
                vec4 ext_b,
                vec4 ext_a,
                out vec4 outcol)
{
  vec4 co = vec4(RANGE_RESCALE(col.rgb, ext_a.x, range.a), layer);
  vec3 samp;
  samp.r = texture(ramp_curve, co.xw).a;
  samp.g = texture(ramp_curve, co.yw).a;
  samp.b = texture(ramp_curve, co.zw).a;

  samp.r = curve_extrapolate(co.x, samp.r, ext_a);
  samp.g = curve_extrapolate(co.y, samp.g, ext_a);
  samp.b = curve_extrapolate(co.z, samp.b, ext_a);

  vec3 rgb_min = vec3(ext_r.x, ext_g.x, ext_b.x);
  co.xyz = RANGE_RESCALE(samp.rgb, rgb_min, range.rgb);

  samp.r = texture(ramp_curve, co.xw).r;
  samp.g = texture(ramp_curve, co.yw).g;
  samp.b = texture(ramp_curve, co.zw).b;

  outcol.r = curve_extrapolate(co.x, samp.r, ext_r);
  outcol.g = curve_extrapolate(co.y, samp.g, ext_g);
  outcol.b = curve_extrapolate(co.z, samp.b, ext_b);
  outcol.a = col.a;

  outcol = mix(col, outcol, fac);
}

void curves_rgb_opti(float fac,
                     vec4 col,
                     float layer,
                     vec4 range,
                     vec4 ext_a,
                     out vec4 outcol)
{
  vec4 co = vec4(RANGE_RESCALE(col.rgb, ext_a.x, range.a), layer);
  vec3 samp;
  samp.r = texture(ramp_curve, co.xw).a;
  samp.g = texture(ramp_curve, co.yw).a;
  samp.b = texture(ramp_curve, co.zw).a;

  outcol.r = curve_extrapolate(co.x, samp.r, ext_a);
  outcol.g = curve_extrapolate(co.y, samp.g, ext_a);
  outcol.b = curve_extrapolate(co.z, samp.b, ext_a);
  outcol.a = col.a;

  outcol = mix(col, outcol, fac);
}


#endif
