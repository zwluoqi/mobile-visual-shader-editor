/*
* Copyright 2021-2021 Zhouwei. All rights reserved.
* License: https://github.com/zwluoqi/mobile-visual-shader-editor#license-bsd-2-clause
*/

#pragma once


#include "BaseDefine.h"

namespace userengine {
    class Light
    {
    public:
        Light(const Vector4& dir, const Vector4& i)
            : m_dir(dir)
            , m_intensity(i)
        {
            u_dir = bgfx::createUniform("uLightDir", bgfx::UniformType::Vec4);
            u_intensity = bgfx::createUniform("uLightRadiance", bgfx::UniformType::Vec4);
            m_deltaTime = 0;
        }
        virtual ~Light() = default;
        
        bool draw_call(float deltaTime,bgfx::ViewId _id, bgfx::ProgramHandle m_program,uint64_t _state = BGFX_STATE_MASK) {
            void *ret =&m_dir;
            bgfx::setUniform(u_dir, ret);
            bgfx::setUniform(u_intensity, &m_intensity);
            return true;
        }
        void destroy() {
            bgfx::destroy(u_dir);
            bgfx::destroy(u_intensity);
        }
        
    private:
        Vector4 m_dir;
        Vector4 m_intensity;
        bgfx::UniformHandle u_dir;
        bgfx::UniformHandle u_intensity;
        float m_deltaTime;
        
        
    };
}
