/*
* Copyright 2021-2021 Zhouwei. All rights reserved.
* License: https://github.com/zwluoqi/mobile-visual-shader-editor#license-bsd-2-clause
*/


#ifndef node_vector_curves_api
#define node_vector_curves_api
//#include "UnityCG.cginc"
#include "../include/base_h.cginc"

/*
向量曲线节点将输入向量分量映射到曲线。
使用这个曲线节点从原始场景中减速或加速。
https://docs.blender.org/manual/en/latest/render/shader_nodes/vector/curves.html

*/

/* ext is vec4(in_x, in_dy, out_x, out_dy). 
做一个校验,检测texture的值小于0或者大于1
*/
float curve_vec_extrapolate(float x, float y, vec4 ext)
{
  if (x < 0.0) {
    return y + x * ext.y;
  }
  else if (x > 1.0) {
    return y + (x - 1.0) * ext.w;
  }
  else {
    return y;
  }
}

#define RANGE_RESCALE(x, min, range) ((x - min) * range)

void node_curves_vec(
                float layer,
                float fac,
                vec3 vec,
                vec3 range,
                vec4 ext_x,
                vec4 ext_y,
                vec4 ext_z,               
                out vec3 outvec)
{

  vec4 co = vec4(vec, layer);

  vec3 xyz_min = vec3(ext_x.x, ext_y.x, ext_z.x);
  co.xyz = RANGE_RESCALE(co.xyz, xyz_min, range);

  outvec.x = texture2D(ramp_curve, co.xw).x;
  outvec.y = texture2D(ramp_curve, co.yw).y;
  outvec.z = texture2D(ramp_curve, co.zw).z;

  outvec.x = curve_vec_extrapolate(co.x, outvec.r, ext_x);
  outvec.y = curve_vec_extrapolate(co.y, outvec.g, ext_y);
  outvec.z = curve_vec_extrapolate(co.z, outvec.b, ext_z);

  outvec = mix(vec, outvec, fac);
}


#endif
