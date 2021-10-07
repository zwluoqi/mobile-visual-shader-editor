/*
* Copyright 2021-2021 Zhouwei. All rights reserved.
* License: https://github.com/zwluoqi/mobile-visual-shader-editor#license-bsd-2-clause
*/


#ifndef node_bump_api
#define node_bump_api
//#include "UnityCG.cginc"
#include "../include/base_h.cginc"


/*

*/
/*
    基于位置信息返回一个参数对x或y的偏导数
    https://www.jianshu.com/p/7fc6a2fef29d
    https://blog.csdn.net/chy555chy/article/details/80177015
    https://en.wikipedia.org/wiki/Bump_mapping
    height作为高度图的输入值 根据bump算法生成新的法线
    用于模拟物体表面的凹凸和褶皱。这是通过扰动物体的表面法线和在光照计算中使用扰动法线来实现的
*/

void dfdx_v3(float3 v, out float3 dx)
{
  dx = v + DFDX_SIGN * dFdx(v);
}

void dfdy_v3(float3 v, out float3 dy)
{
  dy = v + DFDY_SIGN * dFdy(v);
}

/*
v_position 片段MVP的顶点 varying 隐藏节点
N 片段世界空间法线 varying or input
*/
void node_bump(
               float invertVal,// [0,1] to [-1,1]
               float strength,
               float dist,
               float height,
               float3 GPU_WORLD_NORMAL,
               float3 GPU_VIEW_POSITION,
               out float3 result)
{
  float3 vNormal = mul((mat3)(ViewMatrix), normalize(GPU_WORLD_NORMAL));
  invertVal = (invertVal-1)*2+1;
  dist *= invertVal;

  float3 dPdx = dFdx(GPU_VIEW_POSITION);
  float3 dPdy = dFdy(GPU_VIEW_POSITION);
  
  float height_dx = dFdx(height);//原来从外部传过来,现在直接进行偏导数计算
  float height_dy = dFdy(height);

  /* Get surface tangents from normal. */
  float3 Rx = cross(dPdy, vNormal);
  float3 Ry = cross(vNormal, dPdx);

  /* Compute surface gradient and determinant. */
  float det = dot(dPdx, Rx);

  float dHdx = dFdx( height);//may be error with source code(height_dx - height)
  float dHdy = dFdy ( height);
  float3 surfgrad = dHdx * Rx + dHdy * Ry;

  strength = max(strength, 0.0);

  result = normalize(abs(det) * vNormal - dist * sign(det) * surfgrad);
  result = normalize(mix(vNormal, result, strength));

  result = mul((mat3)(ViewMatrixInverse) , result);
}

#endif
