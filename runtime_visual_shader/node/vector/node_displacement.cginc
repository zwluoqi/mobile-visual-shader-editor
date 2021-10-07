/*
* Copyright 2021-2021 Zhouwei. All rights reserved.
* License: https://github.com/zwluoqi/mobile-visual-shader-editor#license-bsd-2-clause
*/


#ifndef node_displacement_api
#define node_displacement_api
//#include "UnityCG.cginc"
#include "../include/base_h.cginc"

/*
https://docs.blender.org/manual/en/latest/render/shader_nodes/vector/displacement.html
置换贴图,只要是根据置换贴图更新了原始的顶点,添加更多的细节效果
顶点根据置换贴图凸起或者内凹,产生部分边缘效果

*:更新了原始的顶点:*
*/
void node_displacement_object(
    float height, float midlevel, float scale, vec3 N,  out vec3 result)
{
  //因为传入的默认法线值是被计算过的世界空间值,先变化到对象空间
  N = mul(vec4(N, 0.0) , objmat).xyz;
  
  //法线延伸
  result = (height - midlevel) * scale * normalize(N);
  
  //对象空间转换到世界空间
  result = mul(objmat , vec4(result, 0.0)).xyz;
}

void node_displacement_world(float height, float midlevel, float scale, vec3 N, out vec3 result)
{
  result = (height - midlevel) * scale * normalize(N);
}


#endif
