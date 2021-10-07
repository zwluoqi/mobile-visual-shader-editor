/*
* Copyright 2021-2021 Zhouwei. All rights reserved.
* License: https://github.com/zwluoqi/mobile-visual-shader-editor#license-bsd-2-clause
*/

#pragma once

#include <vector>
#include <memory>
#include <map>

#include "../shader_online/shader_core/shader_def.h"

namespace userengine {
    static const  bgfx::UniformType::Enum s_shaderRemap[csc::UniformType::Count + 1] ={
        bgfx::UniformType::Sampler,
        bgfx::UniformType::End,
        bgfx::UniformType::Vec4,
        bgfx::UniformType::Mat3,
        bgfx::UniformType::Mat4,
        bgfx::UniformType::Vec4,//vec3
        bgfx::UniformType::Vec4,//vec2
        bgfx::UniformType::Vec4,//float
        bgfx::UniformType::Count,//count
    };

    struct Vec4UniformHandle{
        csc::Float4 vec;
    };

    struct Mat3UniformHandle{
        float vec[9];
    };

    struct Mat4UniformHandle{
        float vec[16];
    };

    struct SamplerUniformHandle{
        bgfx::TextureHandle tex;
        u_int16_t texIndex;
    };


    struct ShaderUniformHandle{
        union UniformHandleValueUnion {
        public:
            UniformHandleValueUnion() {}
            UniformHandleValueUnion(const Vec4UniformHandle& vec4):vec4_value{vec4}{}
            UniformHandleValueUnion(const Mat3UniformHandle& mat3):mat3_value{mat3}{}
            UniformHandleValueUnion(const Mat4UniformHandle& mat4):mat4_value{mat4}{}
            UniformHandleValueUnion(const SamplerUniformHandle& sampler):sampler_value{sampler}{}
            

            Vec4UniformHandle vec4_value;
            Mat3UniformHandle mat3_value;
            Mat4UniformHandle mat4_value;
            SamplerUniformHandle sampler_value;
        };
        
        bgfx::UniformType::Enum _type;
        bgfx::UniformHandle uniformHandle{bgfx::kInvalidHandle};
        UniformHandleValueUnion value_union;
        
        ShaderUniformHandle(const Vec4UniformHandle& vec4){
            _type = bgfx::UniformType::Vec4;
            value_union = vec4;
        }
        ShaderUniformHandle(const Mat3UniformHandle& vec4):_type{bgfx::UniformType::Mat3},value_union{vec4}{}
        ShaderUniformHandle(const Mat4UniformHandle& vec4):_type{bgfx::UniformType::Mat4},value_union{vec4}{}
        ShaderUniformHandle(const SamplerUniformHandle& vec4):_type{bgfx::UniformType::Sampler},value_union{vec4}{}
        ShaderUniformHandle(){}
    };
}
