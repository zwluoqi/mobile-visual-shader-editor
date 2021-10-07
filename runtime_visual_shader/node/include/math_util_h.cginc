/*
* Copyright 2021-2021 Zhouwei. All rights reserved.
* License: https://github.com/zwluoqi/mobile-visual-shader-editor#license-bsd-2-clause
*/

#ifndef math_util_h
#define math_util_h
/* Float Math */


float inversesqrt(float a){
    return 1/sqrt(a);
}
float fract(float a)
{
  return a - floor(a);
}

#ifdef UNITY_CG_INCLUDED
float mod(float x,float y){
    return x - y * floor(x/y);
}
#endif

float safe_divide(float a, float b)
{
  return (b != 0.0) ? a / b : 0.0;
}

/* fmod function compatible with OSL using nvidia reference example. */
float compatible_fmod(float a, float b)
{
  float c = (b != 0.0) ? fract(abs(a / b)) * abs(b) : 0.0;
  return (a < 0.0) ? -c : c;
}

float compatible_pow(float x, float y)
{
  if (y == 0.0) { /* x^0 -> 1, including 0^0 */
    return 1.0;
  }

  /* glsl pow doesn't accept negative x */
  if (x < 0.0) {
    if (mod(-y, 2.0) == 0.0) {
      return pow(-x, y);
    }
    else {
      return -pow(-x, y);
    }
  }
  else if (x == 0.0) {
    return 0.0;
  }

  return pow(x, y);
}

float wrap(float a, float b, float c)
{
  float range = b - c;
  return (range != 0.0) ? a - (range * floor((a - c) / range)) : c;
}

float3 wrap(float3 a, float3 b, float3 c)
{
  return float3(wrap(a.x, b.x, c.x), wrap(a.y, b.y, c.y), wrap(a.z, b.z, c.z));
}

float hypot(float x, float y)
{
  return sqrt(x * x + y * y);
}

int floor_to_int(float x)
{
  return int(floor(x));
}

int quick_floor(float x)
{
  return int(x) - ((x < 0) ? 1 : 0);
}

/* Vector Math */

float2 safe_divide(float2 a, float2 b)
{
  return float2(safe_divide(a.x, b.x), safe_divide(a.y, b.y));
}

float3 safe_divide(float3 a, float3 b)
{
  return float3(safe_divide(a.x, b.x), safe_divide(a.y, b.y), safe_divide(a.z, b.z));
}

float4 safe_divide(float4 a, float4 b)
{
  return float4(
      safe_divide(a.x, b.x), safe_divide(a.y, b.y), safe_divide(a.z, b.z), safe_divide(a.w, b.w));
}

float2 safe_divide(float2 a, float b)
{
  return (b.x != 0.0 ) ? (a / b) : float2(0.0,0.0);
}

float3 safe_divide(float3 a, float b)
{
  return (b != 0.0) ? a / b : float3(0.0,0.0,0.0);
}

float4 safe_divide(float4 a, float b)
{
  return (b != 0.0) ? a / b : float4(0.0,0.0,0.0,0.0);
}

float3 compatible_fmod(float3 a, float3 b)
{
  return float3(compatible_fmod(a.x, b.x), compatible_fmod(a.y, b.y), compatible_fmod(a.z, b.z));
}

void invert_z(float3 v, out float3 outv)
{
  v.z = -v.z;
  outv = v;
}

void vector_normalize(float3 normal, out float3 outnormal)
{
  outnormal = normalize(normal);
}

/* Matirx Math */

float3x3 euler_to_mat3(float3 euler)
{
  float cx = cos(euler.x);
  float cy = cos(euler.y);
  float cz = cos(euler.z);
  float sx = sin(euler.x);
  float sy = sin(euler.y);
  float sz = sin(euler.z);

  float3x3 mat;
  mat[0][0] = cy * cz;
  mat[0][1] = cy * sz;
  mat[0][2] = -sy;

  mat[1][0] = sy * sx * cz - cx * sz;
  mat[1][1] = sy * sx * sz + cx * cz;
  mat[1][2] = cy * sx;

  mat[2][0] = sy * cx * cz + sx * sz;
  mat[2][1] = sy * cx * sz - sx * cz;
  mat[2][2] = cy * cx;
  return mat;
}

void direction_transform_m4v3(float3 vin, float4x4 mat, out float3 vout)
{
  vout = mul(mat , float4(vin, 0.0)).xyz;
}

void normal_transform_transposed_m4v3(float3 vin, float4x4 mat, out float3 vout)
{
  vout = mul(transpose((mat)) , vin);
}

void point_transform_m4v3(float3 vin, float4x4 mat, out float3 vout)
{
  vout = mul(mat , float4(vin, 1.0)).xyz;
}

vec3 safe_normalize(vec3 v)
{
  float len = length(v);
  if (isnan(len) || len == 0.0) {
    return vec3(1.0, 0.0, 0.0);
  }
  return v / len;
}


float3 transform_direction(float4x4 mat,float3 vin){
    float3 vout = mul(mat , float4(vin, 0.0)).xyz;
    return vout;
}


void tangent_orco_x(vec3 orco_in, out vec3 orco_out)
{
  orco_out = orco_in.xzy * vec3(0.0, -0.5, 0.5) + vec3(0.0, 0.25, -0.25);
}

void tangent_orco_y(vec3 orco_in, out vec3 orco_out)
{
  orco_out = orco_in.zyx * vec3(-0.5, 0.0, 0.5) + vec3(0.25, 0.0, -0.25);
}

//z轴塌陷,x y归一化
void tangent_orco_z(vec3 orco_in, out vec3 orco_out)
{
  orco_out = orco_in.yxz * vec3(-0.5, 0.5, 0.0) + vec3(0.25, -0.25, 0.0);
}

void node_tangentmap(vec3 attr_tangent, out vec3 tangent)
{
  tangent = normalize(attr_tangent);
}

void tangent_translate(vec3 N, vec3 orco, mat4 inputObjMat, out vec3 T)
{
  orco = mul(inputObjMat , vec4(orco, 0.0)).xyz;//得到世界空间orco
  T = cross(N, normalize(cross(orco, N)));//旋转180度?
}
    
void invertmat4(mat4 a,inout mat4 ret){
      vec4 a00 = a[0][0],
        a01 = a[0][1],
        a02 = a[0][2],
        a03 = a[0][3];
      vec4 a10 = a[1][0],
        a11 = a[1][1],
        a12 = a[1][2],
        a13 = a[1][3];
      vec4 a20 = a[2][0],
        a21 = a[2][1],
        a22 = a[2][2],
        a23 = a[2][3];
      vec4 a30 = a[3][0],
        a31 = a[3][1],
        a32 = a[3][2],
        a33 = a[3][3];
      vec4 b00 = a00 * a11 - a01 * a10;
      vec4 b01 = a00 * a12 - a02 * a10;
      vec4 b02 = a00 * a13 - a03 * a10;
      vec4 b03 = a01 * a12 - a02 * a11;
      vec4 b04 = a01 * a13 - a03 * a11;
      vec4 b05 = a02 * a13 - a03 * a12;
      vec4 b06 = a20 * a31 - a21 * a30;
      vec4 b07 = a20 * a32 - a22 * a30;
      vec4 b08 = a20 * a33 - a23 * a30;
      vec4 b09 = a21 * a32 - a22 * a31;
      vec4 b10 = a21 * a33 - a23 * a31;
      vec4 b11 = a22 * a33 - a23 * a32;
      // Calculate the determinant
      vec4 det =
        b00 * b11 - b01 * b10 + b02 * b09 + b03 * b08 - b04 * b07 + b05 * b06;
      //if (!det) {
        //return null;
      //}
      det = 1.0 / det;
      ret[0][0] = (a11 * b11 - a12 * b10 + a13 * b09) * det;
      ret[0][1] = (a02 * b10 - a01 * b11 - a03 * b09) * det;
      ret[0][2] = (a31 * b05 - a32 * b04 + a33 * b03) * det;
      ret[0][3] = (a22 * b04 - a21 * b05 - a23 * b03) * det;
      ret[1][0] = (a12 * b08 - a10 * b11 - a13 * b07) * det;
      ret[1][1] = (a00 * b11 - a02 * b08 + a03 * b07) * det;
      ret[1][2] = (a32 * b02 - a30 * b05 - a33 * b01) * det;
      ret[1][3] = (a20 * b05 - a22 * b02 + a23 * b01) * det;
      ret[2][0] = (a10 * b10 - a11 * b08 + a13 * b06) * det;
      ret[2][1] = (a01 * b08 - a00 * b10 - a03 * b06) * det;
      ret[2][2] = (a30 * b04 - a31 * b02 + a33 * b00) * det;
      ret[2][3] = (a21 * b02 - a20 * b04 - a23 * b00) * det;
      ret[3][0] = (a11 * b07 - a10 * b09 - a12 * b06) * det;
      ret[3][1] = (a00 * b09 - a01 * b07 + a02 * b06) * det;
      ret[3][2] = (a31 * b01 - a30 * b03 - a32 * b00) * det;
      ret[3][3] = (a20 * b03 - a21 * b01 + a22 * b00) * det;

}

#endif
