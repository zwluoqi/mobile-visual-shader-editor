/*
 * Copyright 2021-2021 Zhouwei. All rights reserved.
 * License: https://github.com/zwluoqi/mobile-visual-shader-editor#license-bsd-2-clause
 */

#ifndef closure_eval_surface_lib_h
#define closure_eval_surface_lib_h
#include "../include/base_h.cginc"
#include "closure_eval_lib.cginc"


struct BSDFSurfaceOutput
{
    vec3 Albedo;      // diffuse color
    vec3 Specular;    // specular color
    vec3 Normal;      // tangent space normal, if written
    vec3 Emission;
    float Smoothness;    // 0=rough, 1=smooth
    float Occlusion;     // occlusion (default 1)
    float Alpha;        // alpha for transparencies
};


float BSDFSpecularStrength(float3 specular)
{
    return max (max (specular.r, specular.g), specular.b);
}


// Diffuse/Spec Energy conservation
 vec3 BSDFEnergyConservationBetweenDiffuseAndSpecular (float3 albedo, float3 specColor, out float oneMinusReflectivity)
{
        return albedo * (vec3(1,1,1) - specColor);
}

 vec3 BSDFPreMultiplyAlpha (vec3 diffColor, float alpha, float oneMinusReflectivity, out float outModifiedAlpha)
{
    // NOTE: shader relies on pre-multiply alpha-blend (_SrcBlend = One, _DstBlend = OneMinusSrcAlpha)

    // Transparency 'removes' from Diffuse component
    diffColor *= alpha;

    // Reflectivity 'removes' from the rest of components, including Transparency
    // outAlpha = 1-(1-alpha)*(1-reflectivity) = 1-(oneMinusReflectivity - alpha*oneMinusReflectivity) =
    //          = 1-oneMinusReflectivity + alpha*oneMinusReflectivity
    outModifiedAlpha = 1-oneMinusReflectivity + alpha*oneMinusReflectivity;
    return diffColor;
}


//-------------------------------------------------------------------------------------

float BSDFSmoothnessToPerceptualRoughness(float smoothness)
{
    return (1 - smoothness);
}

 float3 BSDFSafeNormalize(float3 inVec)
{
    float dp3 = max(0.001f, dot(inVec, inVec));
    return inVec * rsqrt(dp3);
}

// Note: Disney diffuse must be multiply by diffuseAlbedo / PI. This is done outside of this function.
float BSDFDisneyDiffuse(float NdotV, float NdotL, float LdotH, float perceptualRoughness)
{
    float fd90 = 0.5 + 2 * LdotH * LdotH * perceptualRoughness;
    // Two schlick fresnel term
    float lightScatter   = (1 + (fd90 - 1) * pow(1 - NdotL,5));
    float viewScatter    = (1 + (fd90 - 1) * pow(1 - NdotV,5));

    return lightScatter * viewScatter;
}


float BSDFPerceptualRoughnessToRoughness(float perceptualRoughness)
{
    return perceptualRoughness * perceptualRoughness;
}

// Ref: http://jcgt.org/published/0003/02/03/paper.pdf
 float BSDFSmithJointGGXVisibilityTerm (float NdotL, float NdotV, float roughness)
{
    // Approximation of the above formulation (simplify the sqrt, not mathematically correct but close enough)
    float a = roughness;
    float lambdaV = NdotL * (NdotV * (1 - a) + a);
    float lambdaL = NdotV * (NdotL * (1 - a) + a);

    return 0.5f / (lambdaV + lambdaL + 1e-5f);
}

 float BSDFDistributionGGX (float NdotH, float roughness)
{
    float a2 = roughness * roughness;
    float NdotH2 = NdotH*NdotH;
    float nom   = a2;
    float denom = (NdotH2 * (a2 - 1.0) + 1.0);
    denom = M_PI * denom * denom;
    return nom / max(denom, 0.0001);
}

 vec3 BSDFFresnelTerm (vec3 F0, float cosA)
{
    float t = pow (1 - cosA,5);   // ala Schlick interpoliation
    return F0 + (1-F0) * t;
}

 vec3 BSDFFresnelLerp (vec3 F0, vec3 F90, float cosA)
{
    float t = pow (1 - cosA,5);   // ala Schlick interpoliation
    return lerp (F0, F90, t);
}

// Note: BRDF entry points use smoothness and oneMinusReflectivity for optimization
// purposes, mostly for DX9 SM2.0 level. Most of the math is being done on these (1-x) values, and that saves
// a few precious ALU slots.


// Main Physically Based BRDF
// Derived from Disney work and based on Torrance-Sparrow micro-facet model
//
//   BRDF = kD / pi + kS * (D * V * F) / 4
//   I = BRDF * NdotL
//
// * NDF (depending on UNITY_BRDF_GGX):
//  a) Normalized BlinnPhong
//  b) GGX
// * Smith for Visiblity term
// * Schlick approximation for Fresnel
float4 BSDF_PBS (vec3 diffColor, vec3 specColor, float oneMinusReflectivity, float smoothness,
    float3 normal, float3 viewDir,float3 lightDir,vec4 lightColor, IndirectLight indirect,
    out vec4 diffuseResult,out vec4 speResult,out vec4 surfaceReductionResult)
{
    float perceptualRoughness = BSDFSmoothnessToPerceptualRoughness (smoothness);
    float3 floatDir = BSDFSafeNormalize (lightDir + viewDir);

    float nv = abs(dot(normal, viewDir));    // This abs allow to limit artifact

    float nl = saturate(dot(normal, lightDir));
    float nh = saturate(dot(normal, floatDir));

    float lv = saturate(dot(lightDir, viewDir));
    float lh = saturate(dot(lightDir, floatDir));

    // Diffuse term
    float diffuseTerm = BSDFDisneyDiffuse(nv, nl, lh, perceptualRoughness) * nl;

    // Specular term
    // HACK: theoretically we should divide diffuseTerm by Pi and not multiply specularTerm!
    // BUT 1) that will make shader look significantly darker than Legacy ones
    // and 2) on engine side "Non-important" lights have to be divided by Pi too in cases when they are injected into ambient SH
    float roughness = BSDFPerceptualRoughnessToRoughness(perceptualRoughness);

    // GGX with roughtness to 0 would mean no specular at all, using max(roughness, 0.002) here to match HDrenderloop roughtness remapping.
    roughness = max(roughness, 0.002);
    float grazingTerm = saturate(smoothness + (1-oneMinusReflectivity));

    float V = BSDFSmithJointGGXVisibilityTerm (nl, nv, roughness);
    float D = BSDFDistributionGGX (nh, roughness);
    float3 F = BSDFFresnelTerm (specColor, lh);
    
    float3 specularTermBRDF = V*D*F* M_PI; // Torrance-Sparrow model, Fresnel is applied later

    // specularTerm * nl can be NaN on Metal in some cases, use max() to make sure it's a sane value
    specularTermBRDF = max(0, specularTermBRDF * nl);
    
    // To provide true Lambert lighting, we need to be able to kill specular completely.
    specularTermBRDF *= any(specColor) ? 1.0 : 0.0;

    // surfaceReduction = Int D(NdotH) * NdotH * Id(NdotL>0) dH = 1/(roughness^2+1)
    float surfaceReduction;
    surfaceReduction = 1.0 / (roughness*roughness + 1.0);           // fade \in [0.5;1]

    diffuseResult = vec4(diffColor * (indirect.diffuse + lightColor * diffuseTerm),1.0);
    speResult = vec4(specularTermBRDF * lightColor,1.0);
    surfaceReductionResult = vec4(surfaceReduction * indirect.specular * BSDFFresnelLerp (specColor, grazingTerm, nv),1.0);

    vec3 color =   diffuseResult.xyz
                    + speResult.xyz
                    + surfaceReductionResult.xyz;

    return float4(color, 1);
}


 float4 LightingStandardSpecular (BSDFSurfaceOutput s, float3 viewDir, ClosureLightData light,out vec4 diffuseResult,out vec4 speResult,out vec4 surfaceReductionResult )
{

    s.Normal = normalize(s.Normal);

    // energy conservation
    float oneMinusReflectivity;
    s.Albedo = BSDFEnergyConservationBetweenDiffuseAndSpecular (s.Albedo, s.Specular, /*out*/ oneMinusReflectivity);

    // shader relies on pre-multiply alpha-blend (_SrcBlend = One, _DstBlend = OneMinusSrcAlpha)
    // this is necessary to handle transparency in physically correct way - only diffuse component gets affected by alpha
    float outputAlpha;
    s.Albedo = BSDFPreMultiplyAlpha (s.Albedo, s.Alpha, oneMinusReflectivity, /*out*/ outputAlpha);
    // return vec4(s.Albedo,1.0);

    float4 c = BSDF_PBS (s.Albedo, s.Specular, oneMinusReflectivity, s.Smoothness, s.Normal, viewDir, light.lightDir,light.lightColor, light.indirect,diffuseResult,speResult,surfaceReductionResult);
    
    c.rgb = s.Emission + c.rgb;
    
    c.a = outputAlpha;
    return c;
}

#endif
