/*
* Copyright 2021-2021 Zhouwei. All rights reserved.
* License: https://github.com/zwluoqi/mobile-visual-shader-editor#license-bsd-2-clause
*/


#ifndef node_normal_map_api
#define node_normal_map_api
//#include "UnityCG.cginc"
#include "../include/base_h.cginc"

/*

https://docs.blender.org/manual/en/latest/render/shader_nodes/vector/normal_map.html

*/
//切线空间中的转换
/*
info what?模型缩放信息
tangent 切线坐标的切线向量,一般来自于模型或算法算出
normal 着色对象的世界空间法线
color 输入的扰动法线
*/
void node_normal_map(vec4 info, vec4 tangent, vec3 normal, vec3 texnormal, out vec3 outnormal)
{
  if (all(equal(tangent, vec4(0.0, 0.0, 0.0, 1.0)))) {
    outnormal = normal;
    return;
  }
  // tangent *= (gl_FrontFacing ? 1.0 : -1.0);
  vec3 B = tangent.w * cross(normal, tangent.xyz) * sign(info.w);

  outnormal = texnormal.x * tangent.xyz + texnormal.y * B + texnormal.z * normal;
  outnormal = normalize(outnormal);
}


void color_to_normal_new_shading(vec3 color, out vec3 normal)
{
  normal = vec3(2.0) * color - vec3(1.0);
}

void color_to_blender_normal_new_shading(vec3 color, out vec3 normal)
{
  normal = vec3(2.0, -2.0, -2.0) * color - vec3(1.0);
}

void node_normal_map_mix(float strength, vec3 newnormal, vec3 oldnormal, out vec3 outnormal)
{
  outnormal = normalize(mix(oldnormal, newnormal, max(strength, 0.0)));
}



//输入切线空间颜色,一般是法线贴图,返回世界空间法线
//tangentDir 世界空间切线,隐藏节点
//normalDir 世界空间法线,隐藏节点
void node_normal_map_tangent(float strength, vec3 color,vec4 GPU_OBJECT_INFO,vec3 VERTEX_CD_TANGENT,vec3 GPU_WORLD_NORMAL, out vec3 outnormal){
    vec3 newNormal;
    color_to_normal_new_shading(color,newNormal);
    node_normal_map(GPU_OBJECT_INFO,vec4(VERTEX_CD_TANGENT,1.0),GPU_WORLD_NORMAL,newNormal,outnormal);
    node_normal_map_mix(strength,newNormal,GPU_WORLD_NORMAL,outnormal);
}

//输入世界空间颜色,返回世界空间法线
//tangentDir 世界空间切线,隐藏节点
//normalDir 世界空间法线,隐藏节点
void node_normal_map_world(float strength, vec3 color,vec4 GPU_OBJECT_INFO,vec3 VERTEX_CD_TANGENT,vec3 GPU_WORLD_NORMAL,out vec3 outnormal){
    vec3 newNormal;
    color_to_blender_normal_new_shading(color,newNormal);

    node_normal_map_mix(strength,newNormal,GPU_WORLD_NORMAL,outnormal);
}

//输入对象空间颜色,返回世界空间法线
//tangentDir 世界空间切线,隐藏节点
//normalDir 世界空间法线,隐藏节点
void node_normal_map_obj(float strength, vec3 color,vec4 GPU_OBJECT_INFO,vec3 VERTEX_CD_TANGENT,vec3 GPU_WORLD_NORMAL,out vec3 outnormal){

    vec3 newNormal;
    color_to_blender_normal_new_shading(color,newNormal);
    newNormal = mul(ModelMatrix,newNormal);
    node_normal_map_mix(strength,newNormal,GPU_WORLD_NORMAL,outnormal);
}


#endif
