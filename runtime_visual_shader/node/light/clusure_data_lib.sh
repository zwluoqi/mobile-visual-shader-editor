/*
* Copyright 2021-2021 Zhouwei. All rights reserved.
* License: https://github.com/zwluoqi/mobile-visual-shader-editor#license-bsd-2-clause
*/

#ifndef closure_data_lib_h
#define closure_data_lib_h


#include "closure_type_lib.cginc"

//1.没有光照探针
//2.没有网格光源
//3.没有面光源
//4.没有光源遮挡shadowmask

//-------------------------------------------------------------------------------------
// Specular workflow


/* Common cl_out struct used by most closures. */
struct ClosureOutput {
  vec3 radiance;
};
struct OcclusionData {
  /* 4 horizons angles, one in each direction around the view vector to form a cross pattern. */
  vec4 horizons;
  /* Custom large scale occlusion. */
  float custom_occlusion;
};
#define NO_OCCLUSION_DATA(t0) \
  t0.horizons = vec4(M_PI, -M_PI, M_PI, -M_PI);\
  t0.custom_occlusion  = 1.0;

struct ClosureInputCommon {
  /** Custom occlusion value set by the user. */
  float occlusion;
  vec3 worldPosition;
  vec3 viewPosition; /* Required. otherwise generate linking error. */
  vec3 worldNormal;  /* Required. otherwise generate linking error. */
  vec3 viewNormal;   /* Required. otherwise generate linking error. */
  vec3 gl_FragCoord;
};
//计算着色点信息
struct ClosureEvalCommon {
  /** Result of SSAO. */
  OcclusionData occlusion_data;
  /** View vector. */
  vec3 V;
  /** Surface position. */
  vec3 P;
  /** Normal vector, always facing camera. */
  vec3 N;
  /** Normal vector, always facing camera. (viewspace) */
  vec3 vN;
  /** Surface position. (viewspace) */
  vec3 vP;
  /** Geometric normal, always facing camera. */
  vec3 Ng;
  /** Geometric normal, always facing camera. (viewspace) */
  vec3 vNg;
  /** Random numbers. 3 random sequences. zw is a random point on a circle. */
  vec4 rand;
  /** Specular probe accumulator. Shared between planar and cubemap probe. */
  float specular_accum;
  /** Diffuse probe accumulator. */
  float diffuse_accum;
};

struct IndirectLight
{
    half3 diffuse;
    half3 specular;
};

/* convenience aliases */
#define l_color color.rgb
#define l_position position.xyz
#define l_influence position.w
#define l_influence_volume color.w

//着色点光照信息
struct ClosureLightData {
  vec3 lightDir;
  vec4 lightColor;
  IndirectLight indirect;
};

#endif
