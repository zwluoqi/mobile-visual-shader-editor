/*
* Copyright 2021-2021 Zhouwei. All rights reserved.
* License: https://github.com/zwluoqi/mobile-visual-shader-editor#license-bsd-2-clause
*/

#ifndef node_clamp_h
#define node_clamp_h

/*
    clamp值
*/

void clamp_value(float value, float min, float max, out float result)
{
  result = clamp(value, min, max);
}

void clamp_minmax(float value, float min_allowed, float max_allowed, out float result)
{
  result = min(max(value, min_allowed), max_allowed);
}

void clamp_range(float value, float min, float max, out float result)
{
  result = (max > min) ? clamp(value, min, max) : clamp(value, max, min);
}

#endif
