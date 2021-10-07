/*
* Copyright 2021-2021 Zhouwei. All rights reserved.
* License: https://github.com/zwluoqi/mobile-visual-shader-editor#license-bsd-2-clause
*/

#ifndef share_uniform_h
#define share_uniform_h

uniform float4x4 uLightMVP;
uniform float4x4 uLightVP;
uniform float4 uLightDir;
uniform float4 uLightPos;
uniform float4 uLightRadiance;
uniform float4 GPU_OBJECT_COLOR;//from engine settting nothing todo for bgfx
uniform float4 GPU_OBJECT_INFO;//from engine settting nothing todo for bgfx,z=hue,x=obj_index,w=flag
uniform float4 GPU_OBJECT_MAT_INDEX;//from engine settting nothing todo for bgfx

// uniform float2 GPU_VIEW_POSITION;
// uniform float4 GPU_WORLD_NORMAL;
// uniform float4 GPU_WORLD_POSITION;

// uniform float3 VERTEX_CD_ORCO;
// uniform float3 VERTEX_CD_MTFACE;//UV 坐标
// uniform float3 VERTEX_CD_MCOL;// Color
// uniform float3 VERTEX_CD_TANGENT;

#endif
