/*
* Copyright 2021-2021 Zhouwei. All rights reserved.
* License: https://github.com/zwluoqi/mobile-visual-shader-editor#license-bsd-2-clause
*/


#ifndef node_vector_transform_api
#define node_vector_transform_api
//#include "UnityCG.cginc"
#include "../include/base_h.cginc"

/*
https://docs.blender.org/manual/en/latest/render/shader_nodes/vector/transform.html
*/
//#define ViewMatrix UNITY_MATRIX_V
//#define ProjectorMatrix UNITY_MATRIX_P
//#define ModelMatrix UNITY_MATRIX_M

void transform_point_world_object(vec3 p,out vec3 ret){
    ret = mul(inv_objmat,vec4(p,1.0)).xyz;
}

void transform_point_object_world(vec3 p,out vec3 ret){
    ret = mul(ModelMatrix,vec4(p,1.0)).xyz;
}

void transform_point_camera_world(vec3 p,out vec3 ret){
    ret = mul(ViewMatrixInverse,vec4(p,1.0)).xyz;
}

void transform_point_world_camera(vec3 p,out vec3 ret){
    ret = mul(ViewMatrix,vec4(p,1.0)).xyz;
}

void transform_point_object_camera(vec3 p,out vec3 ret){
    transform_point_object_world(p,ret);
    transform_point_world_camera(ret,ret);
}

void transform_point_camera_object(vec3 p,out vec3 ret){
    transform_point_camera_world(p,ret);
    transform_point_world_object(ret,ret);
}


void transform_vector_object_world(vec3 dir,out vec3 ret){
    ret = mul(ModelMatrix,vec4(dir,0.0)).xyz;
}

void transform_vector_world_object(vec3 dir,out vec3 ret){
    ret = mul(inv_objmat,vec4(dir,0.0)).xyz;
}

void transform_vector_world_camera(vec3 dir,out vec3 ret){
    ret = mul(ViewMatrix,vec4(dir,0.0)).xyz;
}

void transform_vector_camera_world(vec3 dir,out vec3 ret){
    ret = mul(ViewMatrixInverse,vec4(dir,0.0)).xyz;
}

void transform_vector_object_camera(vec3 dir,out vec3 ret){
    transform_vector_object_world(dir,ret);
    transform_vector_world_camera(ret,ret);
}

void transform_vector_camera_object(vec3 dir,out vec3 ret){
    transform_vector_camera_world(dir,ret);
    transform_vector_world_object(ret,ret);
}


void transform_normal_object_world(vec3 normal,out vec3 ret){
 mat4 inv_trans = transpose((inv_objmat));
 ret = mul(inv_trans,vec4(normal,0.0)).xyz;
}

void transform_normal_world_camera(vec3 normal,out vec3 ret){
 mat4 inv_trans = transpose((ViewMatrixInverse));
 ret = mul(inv_trans,vec4(normal,0.0)).xyz;
}

void transform_normal_object_camera(vec3 normal,out vec3 ret){
    transform_normal_object_world(normal,ret);
    transform_normal_world_camera(ret,ret);
}

#endif
