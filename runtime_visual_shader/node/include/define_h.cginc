/*
* Copyright 2021-2021 Zhouwei. All rights reserved.
* License: https://github.com/zwluoqi/mobile-visual-shader-editor#license-bsd-2-clause
*/


#ifndef define_h
#define define_h


#include "share_uniform.cginc"

#define DFDX_SIGN 1.0f
#define DFDY_SIGN -1.0f

#define POINT 0.0
#define SUN 1.0
#define SPOT 2.0
#define AREA_RECT 4.0
/* Used to define the area light shape, doesn't directly correspond to a Blender light type. */
#define AREA_ELLIPSE 100.0



//vertex attribute
//VERTEX_CD_MTFACE  UV Map
//VERTEX_CD_MCOL color
//VERTEX_CD_ORCO original coordinates default zero,顶点坐标？
//VERTEX_CD_TANGENT tangle

//GPU_BARYCENTRIC_TEXCO 重心坐标
//GPU_BARYCENTRIC_DIST 重心距离?

#ifdef UNITY_CG_INCLUDED
#define M_PI 3.1415926

#define ViewMatrix UNITY_MATRIX_V
#define ProjectionMatrix UNITY_MATRIX_P
#define ModelMatrix UNITY_MATRIX_M
#define ViewMatrixInverse UNITY_MATRIX_I_V
#define ViewProjectorMatrix UNITY_MATRIX_VP
#define u_proj UNITY_MATRIX_P
#define objmat UNITY_MATRIX_M
#define inv_objmat unity_WorldToObject
#define viewmat UNITY_MATRIX_V
#define view2worldmat UNITY_MATRIX_V
//#define ProjectionMatrixInverse UNITY_MATRIX_I_P
// #define ModelMatrixInverse unity_WorldToObject

#define mat4 float4x4
#define mat3 float3x3
#define vec3 float3
#define vec2 float2
#define vec4 float4
#define dFdx ddx
#define dFdy ddy
#define mix lerp



#define sampler1DArray sampler2D

#define texture tex2D
#define texture2D tex2D
#define texelFetch(samplerName,tuv,tlod) tex2Dlod(samplerName,float4(tuv,0.0,tlod))
#define texelFetch3D(samplerName,txyz,tlod) tex3Dlod(samplerName,float4(txyz,tlod))

#define texture3D tex3D



/*
#define UNITY_MATRIX_P glstate_matrix_projection
#define UNITY_MATRIX_V unity_MatrixV
#define UNITY_MATRIX_I_V unity_MatrixInvV
#define UNITY_MATRIX_VP unity_MatrixVP
#define UNITY_MATRIX_M unity_ObjectToWorld
#define UNITY_MATRIX_I_M unity_WorldToObject

*/


#else



#define ViewMatrix u_view
#define ProjectionMatrix u_proj
#define ModelMatrix u_model[0]
#define ViewMatrixInverse u_invView
#define ViewProjectorMatrix u_modelViewProj
//#define ProjectionMatrixInverse u_invProj

#define objmat u_model[0]
#define viewmat u_view
#define inv_objmat u_model[1]
#define view2worldmat u_invView
// #define ModelMatrixInverse unity_WorldToObject

#define sampler1DArray BgfxSampler2D
#define texture bgfxTexture2D
//#define texture3D bgfxTexture3D
//#define texelFetch(samplerName,tuv,tlod) tex2Dlod(samplerName,float4(tuv,0.0,tlod))
#define texelFetch3D texelFetch


// uniform vec4  u_viewRect;
// uniform vec4  u_viewTexel;
// uniform mat4  u_view;
// uniform mat4  u_invView;
// uniform mat4  u_proj;
// uniform mat4  u_invProj;
// uniform mat4  u_viewProj;
// uniform mat4  u_invViewProj;
// uniform mat4  u_model[BGFX_CONFIG_MAX_BONES];
// uniform mat4  u_modelView;
// uniform mat4  u_modelViewProj;
// uniform vec4  u_alphaRef4;

#endif


#define M_PI_2 M_PI*2
#define M_PI_4 M_PI*4
#define INV_PI (1/M_PI)

#define cameraForward ViewMatrixInverse[2].xyz
#define cameraPos ViewMatrixInverse[3].xyz
#define cameraVec(P) ((ProjectionMatrix[3][3] == 0.0) ? normalize(cameraPos - P) : cameraForward)


#endif
