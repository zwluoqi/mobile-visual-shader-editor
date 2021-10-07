/*
* Copyright 2021-2021 Zhouwei. All rights reserved.
* License: https://github.com/zwluoqi/mobile-visual-shader-editor#license-bsd-2-clause
*/

#ifndef node_bsdf_surface_h
#define node_bsdf_surface_h

#include "../include/base_h.cginc"
#include "../light/eval_surface_lib.cginc"


void node_bsdf_pbr(vec4 albedo,vec4 specular, vec4 emission,float smoothness, vec3 GPU_WORLD_NORMAL,vec3 GPU_WORLD_POSITION,out Closure result
                    ,out vec4 diffuseResult,out vec4 speResult,out vec4 surfaceReductionResult){
    BSDFSurfaceOutput surface;
    surface.Albedo = albedo.xyz;
    surface.Specular = specular.xyz;
    surface.Normal = GPU_WORLD_NORMAL;
    surface.Emission = emission.xyz;
    surface.Alpha = 1.0;
    vec3 viewDir = cameraVec(GPU_WORLD_POSITION);
    
    ClosureLightData lightData;
    lightData.lightDir = uLightDir.xyz;
    lightData.lightColor = vec4(1);
    vec4 radiance = LightingStandardSpecular(surface,viewDir,lightData,diffuseResult,speResult,surfaceReductionResult);
    result.radiance = radiance;
}


// void node_bsdf_pbr(vec4 albedo,float smoothness, vec3 GPU_WORLD_NORMAL,vec3 GPU_WORLD_POSITION,out Closure result){
//     BSDFSurfaceOutput surface;
//     surface.Albedo = albedo.xyz;
//     // surface.Specular = specular.xyz;
//     surface.Normal = GPU_WORLD_NORMAL;
//     // surface.Emission = emission.xyz;
//     vec3 viewDir = cameraVec(GPU_WORLD_POSITION);
    
//     ClosureLightData lightData;
//     lightData.lightDir = uLightDir.xyz;
//     lightData.lightColor = vec4(1);
//     vec4 radiance = LightingStandardSpecular(surface,viewDir,lightData);
//     result.radiance = albedo;
// }

#endif