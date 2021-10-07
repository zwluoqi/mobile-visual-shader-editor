/*
* Copyright 2021-2021 Zhouwei. All rights reserved.
* License: https://github.com/zwluoqi/mobile-visual-shader-editor#license-bsd-2-clause
*/


#ifndef node_vector_displacement_api
#define node_vector_displacement_api
//#include "UnityCG.cginc"
#include "../include/base_h.cginc"

/*
https://docs.blender.org/manual/en/latest/render/shader_nodes/vector/vector_displacement.html
相对于displacement去变化已有法线,
这个直接使用输入的dir作为法线值去变化
所以理论上使用node_vector_displacement_tangent连接法线贴图,直接得到最终的世界坐标法线向量是这个的主要用途
*/
//这个接口是不是应该在vs中计算?因为需要原始的法线和切线信息
void node_vector_displacement_tangent(vec3 dir,//法线贴图
                                      float midlevel,
                                      float scale,
                                      vec4 VERTEX_CD_TANGENT,//顶点切线
                                      vec3 GPU_WORLD_NORMAL,//内部世界空间法线                                      
                                      out vec3 result)
{
  /* TODO(fclem): this is broken. revisit latter. */
  vec3 N_object = normalize(mul(mul(vec4(GPU_WORLD_NORMAL, 0.0) , viewmat) , objmat).xyz);
  vec3 T_object = normalize(mul(mul(vec4(VERTEX_CD_TANGENT.xyz, 0.0) , viewmat) , objmat).xyz);
  vec3 B_object = VERTEX_CD_TANGENT.w * normalize(cross(N_object, T_object));

  vec3 offset = (dir.xyz - vec3(midlevel,midlevel,midlevel)) * scale;
  
  //经过TNB变化来到对象空间
  result = offset.x * T_object + offset.y * N_object + offset.z * B_object;
  
  //经过obmat变化来到世界空间
  result = mul(objmat , vec4(result, 0.0)).xyz;
}

void node_vector_displacement_object(
                                    vec3 dir, 
                                    float midlevel, 
                                    float scale,
                                    vec4 VERTEX_CD_TANGENT,//顶点法线
                                    vec3 GPU_WORLD_NORMAL,//内部世界空间法线  
                                     out vec3 result)
{  
  result = (dir.xyz - vec3(midlevel,midlevel,midlevel)) * scale;
  
  //得到世界空间的值
  result = mul(objmat , vec4(result, 0.0)).xyz;
}

void node_vector_displacement_world(vec3 dir, 
                                    float midlevel, 
                                    float scale, 
                                    vec4 VERTEX_CD_TANGENT,//顶点法线
                                    vec3 GPU_WORLD_NORMAL,//内部世界空间法线     
                                    out vec3 result)
{
  result = (dir.xyz - vec3(midlevel,midlevel,midlevel)) * scale;
}


#endif
