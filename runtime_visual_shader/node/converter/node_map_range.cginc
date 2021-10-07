/*
* Copyright 2021-2021 Zhouwei. All rights reserved.
* License: https://github.com/zwluoqi/mobile-visual-shader-editor#license-bsd-2-clause
*/


#ifndef map_range_api
#define map_range_api
//#include "UnityCG.cginc"
#include "../include/base_h.cginc"


float smootherstep(float edge0, float edge1, float ix)
{
  float x = clamp(safe_divide((ix - edge0), (edge1 - edge0)), 0.0, 1.0);
  return x * x * x * (x * (x * 6.0 - 15.0) + 10.0);
}

void map_range_linear(float value,
                      float fromMin,
                      float fromMax,
                      float toMin,
                      float toMax,
                      float steps,
                      out float result)
{
  if (fromMax != fromMin) {
    result = toMin + ((value - fromMin) / (fromMax - fromMin)) * (toMax - toMin);
  }
  else {
    result = 0.0;
  }
}

void map_range_stepped(float value,
                       float fromMin,
                       float fromMax,
                       float toMin,
                       float toMax,
                       float steps,
                       out float result)
{
  if (fromMax != fromMin) {
    float factor = (value - fromMin) / (fromMax - fromMin);
    factor = (steps > 0.0) ? floor(factor * (steps + 1.0)) / steps : 0.0;
    result = toMin + factor * (toMax - toMin);
  }
  else {
    result = 0.0;
  }
}

void map_range_smoothstep(float value,
                          float fromMin,
                          float fromMax,
                          float toMin,
                          float toMax,
                          float steps,
                          out float result)
{
  if (fromMax != fromMin) {
    float factor = (fromMin > fromMax) ? 1.0 - smoothstep(fromMax, fromMin, value) :
                                         smoothstep(fromMin, fromMax, value);
    result = toMin + factor * (toMax - toMin);
  }
  else {
    result = 0.0;
  }
}

void map_range_smootherstep(float value,
                            float fromMin,
                            float fromMax,
                            float toMin,
                            float toMax,
                            float steps,
                            out float result)
{
  if (fromMax != fromMin) {
    float factor = (fromMin > fromMax) ? 1.0 - smootherstep(fromMax, fromMin, value) :
                                         smootherstep(fromMin, fromMax, value);
    result = toMin + factor * (toMax - toMin);
  }
  else {
    result = 0.0;
  }
}

#endif
