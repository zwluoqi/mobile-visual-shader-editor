/*
* Copyright 2021-2021 Zhouwei. All rights reserved.
* License: https://github.com/zwluoqi/mobile-visual-shader-editor#license-bsd-2-clause
*/

#ifndef node_wavelength_h
#define node_wavelength_h

/*
    https://docs.blender.org/manual/en/latest/render/shader_nodes/converter/wavelength.html
*/
//limited 380-780
void node_wavelength(float wavelength,
                     float layer,
                     out vec4 color)
{
  //mat3 xyz_to_rgb = mat3(xyz_to_r, xyz_to_g, xyz_to_b);
  float t = (wavelength - 380.0) / (780.0 - 380.0);
  vec3 xyz = texture2D(ramp_curve, vec2(t, layer)).rgb;
  vec3 rgb = xyz_to_rgb(xyz);
  rgb *= 1.0 / 2.52; /* Empirical scale from lg to make all comps <= 1. */
  color = vec4(clamp(rgb, 0.0, 1.0), 1.0);
}

/*
void Unity_NoiseSineWave_float4(float4 In, float2 MinMax, out float4 Out)
{
    float sinIn = sin(In);
    float sinInOffset = sin(In + 1.0);
    float randomno =  frac(sin((sinIn - sinInOffset) * (12.9898 + 78.233))*43758.5453);
    float noise = lerp(MinMax.x, MinMax.y, randomno);
    Out = sinIn + noise;
}
*/

#endif
