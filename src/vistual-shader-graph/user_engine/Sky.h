/*
* Copyright 2021-2021 Zhouwei. All rights reserved.
* License: https://github.com/zwluoqi/mobile-visual-shader-editor#license-bsd-2-clause
*/

#pragma once

#include "BaseDefine.h"

namespace userengine {


    struct SkyPosColorTexCoord0Vertex
    {
        float m_x;
        float m_y;
        float m_z;
        uint32_t m_rgba;
        float m_u;
        float m_v;
    };


   class Sky
   {
        public:
            Sky();
            void draw_call(uint16_t viewId,float *mtx,float m_width,float m_height);
            void destroy();
        private:
            bgfx::ProgramHandle m_skyProgram;
            bgfx::TextureHandle m_uffizi;
            bgfx::UniformHandle s_texCube;
            bgfx::UniformHandle u_mtx;
            bgfx::VertexLayout ms_sky_layout;
            float s_texelHalf = 0.0f;

            void screen_space_quad(float _textureWidth, float _textureHeight, bool _originBottomLeft = false, float _width = 1.0f, float _height = 1.0f);
   };
}
