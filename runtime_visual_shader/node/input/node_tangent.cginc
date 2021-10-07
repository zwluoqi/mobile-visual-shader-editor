/*
* Copyright 2021-2021 Zhouwei. All rights reserved.
* License: https://github.com/zwluoqi/mobile-visual-shader-editor#license-bsd-2-clause
*/


#ifndef node_tangent_api
#define node_tangent_api
//#include "UnityCG.cginc"
#include "../include/base_h.cginc"
/*
orco:旋转轴
*/
//输入对象空间切平面方向
/* tangent */
#define SHD_TANGENT_RADIAL 0
#define SHD_TANGENT_UVMAP 1

/* tangent */
#define SHD_TANGENT_AXIS_X 0
#define SHD_TANGENT_AXIS_Y 1
#define SHD_TANGENT_AXIS_Z 2

//这里如果有设置,应该去切线贴图采样,
//如果没有设置,直接从mesh取tangent
void node_tangent(float direction_type, 
                    float axis,
                    vec3 VERTEX_CD_TANGENT,
                    vec3 VERTEX_CD_ORCO, 
                    vec3 GPU_WORLD_NORMAL, 
                    out vec3 T)
{
  if(direction_type == SHD_TANGENT_UVMAP){
    node_tangentmap(VERTEX_CD_TANGENT,T);
  }else{
    if (axis == SHD_TANGENT_AXIS_X) {
        tangent_orco_x(VERTEX_CD_ORCO,VERTEX_CD_ORCO);
    }
    else if (axis == SHD_TANGENT_AXIS_Y) {
        tangent_orco_y(VERTEX_CD_ORCO,VERTEX_CD_ORCO);
    }
    else {
        tangent_orco_z(VERTEX_CD_ORCO,VERTEX_CD_ORCO);
    }
    tangent_translate(GPU_WORLD_NORMAL,VERTEX_CD_ORCO,objmat,T);
  }
}



#endif
