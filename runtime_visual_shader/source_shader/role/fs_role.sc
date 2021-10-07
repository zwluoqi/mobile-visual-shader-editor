$input v_color0,v_texcoord0

/*
 * Copyright 2011-2021 Branimir Karadzic. All rights reserved.
 * License: https://github.com/bkaradzic/bgfx#license-bsd-2-clause
 */

#include "../common/common.sh"

SAMPLER2D(texture_diffuse,  0);
SAMPLER2D(texture_specular, 1);
SAMPLER2D(texture_normal,  2);
SAMPLER2D(texture_emissive, 3);

void main()
{
	vec4 color = toLinear(texture2D(texture_diffuse, v_texcoord0) );
	gl_FragColor = color;
}
