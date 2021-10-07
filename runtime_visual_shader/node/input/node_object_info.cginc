/*
* Copyright 2021-2021 Zhouwei. All rights reserved.
* License: https://github.com/zwluoqi/mobile-visual-shader-editor#license-bsd-2-clause
*/


#ifndef node_object_info_api
#define node_object_info_api
//#include "UnityCG.cginc"
#include "../include/base_h.cginc"
/*
GPU_OBJECT_INFO?
*/
void node_object_info(vec4 GPU_OBJECT_COLOR,
                      vec4 GPU_OBJECT_INFO,
                      float GPU_OBJECT_MAT_INDEX,
                      out vec3 location,
                      out vec4 color,
                      out float object_index,
                      out float material_index,
                      out float random)
{
  location = objmat[3].xyz;
  color = GPU_OBJECT_COLOR;
  object_index = GPU_OBJECT_INFO.x;
  material_index = GPU_OBJECT_MAT_INDEX;
  random = GPU_OBJECT_INFO.z;
}



#endif
