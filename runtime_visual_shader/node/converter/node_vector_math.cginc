/*
* Copyright 2021-2021 Zhouwei. All rights reserved.
* License: https://github.com/zwluoqi/mobile-visual-shader-editor#license-bsd-2-clause
*/


#ifndef node_vector_math_api
#define node_vector_math_api
//#include "UnityCG.cginc"
#include "../include/base_h.cginc"

void vector_math_add(float3 a, float3 b, float3 c, float scale, out float3 outVector, out float outValue)
{
  outVector = a + b;
}

void vector_math_subtract(
    float3 a, float3 b, float3 c, float scale, out float3 outVector, out float outValue)
{
  outVector = a - b;
}

void vector_math_multiply(
    float3 a, float3 b, float3 c, float scale, out float3 outVector, out float outValue)
{
  outVector = a * b;
}

void vector_math_divide(
    float3 a, float3 b, float3 c, float scale, out float3 outVector, out float outValue)
{
  outVector = safe_divide(a, b);
}

void vector_math_cross(float3 a, float3 b, float3 c, float scale, out float3 outVector, out float outValue)
{
  outVector = cross(a, b);
}

void vector_math_project(
    float3 a, float3 b, float3 c, float scale, out float3 outVector, out float outValue)
{
  float lenSquared = dot(b, b);
  outVector = (lenSquared != 0.0) ? (dot(a, b) / lenSquared) * b : float3(0.0,0.0,0.0);
}

void vector_math_reflect(
    float3 a, float3 b, float3 c, float scale, out float3 outVector, out float outValue)
{
  outVector = reflect(a, normalize(b));
}

void vector_math_dot(float3 a, float3 b, float3 c, float scale, out float3 outVector, out float outValue)
{
  outValue = dot(a, b);
}

void vector_math_distance(
    float3 a, float3 b, float3 c, float scale, out float3 outVector, out float outValue)
{
  outValue = distance(a, b);
}

void vector_math_length(
    float3 a, float3 b, float3 c, float scale, out float3 outVector, out float outValue)
{
  outValue = length(a);
}

void vector_math_scale(float3 a, float3 b, float3 c, float scale, out float3 outVector, out float outValue)
{
  outVector = a * scale;
}

void vector_math_normalize(
    float3 a, float3 b, float3 c, float scale, out float3 outVector, out float outValue)
{
  outVector = a;
  /* Safe version of normalize(a). */
  float lenSquared = dot(a, a);
  if (lenSquared > 0.0) {
    outVector *= inversesqrt(lenSquared);
  }
}

void vector_math_snap(float3 a, float3 b, float3 c, float scale, out float3 outVector, out float outValue)
{
  outVector = floor(safe_divide(a, b)) * b;
}

void vector_math_floor(float3 a, float3 b, float3 c, float scale, out float3 outVector, out float outValue)
{
  outVector = floor(a);
}

void vector_math_ceil(float3 a, float3 b, float3 c, float scale, out float3 outVector, out float outValue)
{
  outVector = ceil(a);
}

void vector_math_modulo(
    float3 a, float3 b, float3 c, float scale, out float3 outVector, out float outValue)
{
  outVector = compatible_fmod(a, b);
}

void vector_math_wrap(float3 a, float3 b, float3 c, float scale, out float3 outVector, out float outValue)
{
  outVector = wrap(a, b, c);
}

void vector_math_fraction(
    float3 a, float3 b, float3 c, float scale, out float3 outVector, out float outValue)
{
  outVector = fract(a);
}

void vector_math_absolute(
    float3 a, float3 b, float3 c, float scale, out float3 outVector, out float outValue)
{
  outVector = abs(a);
}

void vector_math_minimum(
    float3 a, float3 b, float3 c, float scale, out float3 outVector, out float outValue)
{
  outVector = min(a, b);
}

void vector_math_maximum(
    float3 a, float3 b, float3 c, float scale, out float3 outVector, out float outValue)
{
  outVector = max(a, b);
}

void vector_math_sine(float3 a, float3 b, float3 c, float scale, out float3 outVector, out float outValue)
{
  outVector = sin(a);
}

void vector_math_arcsine(float3 a, float3 b, float3 c, float scale, out float3 outVector, out float outValue)
{
  outVector = asin(a);
}

void vector_math_cosine(
    float3 a, float3 b, float3 c, float scale, out float3 outVector, out float outValue)
{
  outVector = cos(a);
}

void vector_math_arccosine(
    float3 a, float3 b, float3 c, float scale, out float3 outVector, out float outValue)
{
  outVector = acos(a);
}

void vector_math_tangent(
    float3 a, float3 b, float3 c, float scale, out float3 outVector, out float outValue)
{
  outVector = tan(a);
}

void vector_math_arctangent(
    float3 a, float3 b, float3 c, float scale, out float3 outVector, out float outValue)
{
  outVector = atan(a);
}

void vector_math_refract(
    float3 a, float3 b, float3 c, float scale, out float3 outVector, out float outValue)
{
  outVector = refract(a, normalize(b), scale);
}

void vector_math_faceforward(
    float3 a, float3 b, float3 c, float scale, out float3 outVector, out float outValue)
{
  outVector = faceforward(a, b, c);
}

void vector_math_multiply_add(
    float3 a, float3 b, float3 c, float scale, out float3 outVector, out float outValue)
{
  outVector = a * b + c;
}



#endif
