/*
* Copyright 2021-2021 Zhouwei. All rights reserved.
* License: https://github.com/zwluoqi/mobile-visual-shader-editor#license-bsd-2-clause
*/


#ifndef node_light_falloff_api
#define node_light_falloff_api
//#include "UnityCG.cginc"
#include "../include/base_h.cginc"

/*

不支持,因为没有点光源

*/
void node_light_falloff(float Strength ,
                          float Smooth ,
                          out float Quadratic ,
                          out float Linear ,
                          out float Constant )
{
  // float ray_length = 0.0;
  float strength = Strength;
  float ray_length = 1.0;
  //getattribute("path:ray_length", ray_length);

  if (Smooth > 0.0) {
    float squared = ray_length * ray_length;
    strength *= squared / (Smooth + squared);
  }

  /* Quadratic */
  Quadratic = strength;

  /* Linear */
  Linear = (strength * ray_length);

  /* Constant */
  Constant = (strength * ray_length * ray_length);
}

#endif
