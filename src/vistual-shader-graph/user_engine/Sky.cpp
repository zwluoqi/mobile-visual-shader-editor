/*
* Copyright 2021-2021 Zhouwei. All rights reserved.
* License: https://github.com/zwluoqi/mobile-visual-shader-editor#license-bsd-2-clause
*/

#include "vsg_global.h"
#include "Sky.h"

namespace userengine {

    Sky::Sky(){
        ms_sky_layout
           .begin()
           .add(bgfx::Attrib::Position,  3, bgfx::AttribType::Float)
           .add(bgfx::Attrib::Color0,    4, bgfx::AttribType::Uint8, true)
           .add(bgfx::Attrib::TexCoord0, 2, bgfx::AttribType::Float)
           .end();
       
        u_mtx       = bgfx::createUniform("u_mtx",      bgfx::UniformType::Mat4);
        vsg::compile_shader("source_shader/sky/vs_hdr_skybox.sc","vs_hdr_skybox","vertex");
        vsg::compile_shader("source_shader/sky/fs_hdr_skybox.sc","fs_hdr_skybox","fragment");
        m_skyProgram = vsg::load_shader_program("vs_hdr_skybox",  "fs_hdr_skybox");
        s_texCube   = bgfx::createUniform("s_texCube",  bgfx::UniformType::Sampler);
        m_uffizi = loadTexture("textures/uffizi.ktx"
               , 0
               | BGFX_SAMPLER_U_CLAMP
               | BGFX_SAMPLER_V_CLAMP
               | BGFX_SAMPLER_W_CLAMP
               );
        auto m_caps = bgfx::getCaps();
        s_texelHalf = bgfx::RendererType::Direct3D9 == m_caps->rendererType ? 0.5f : 0.0f;
    }

    void Sky::destroy(){
        bgfx::destroy(m_skyProgram);
        bgfx::destroy(m_uffizi);
    }

    void Sky::draw_call(uint16_t viewId,float *mtx,float m_width,float m_height){
       uint64_t _state = 0
       | BGFX_STATE_WRITE_RGB
       | BGFX_STATE_WRITE_A
    //           | BGFX_STATE_WRITE_Z
    //           | BGFX_STATE_DEPTH_TEST_LESS
    //           | BGFX_STATE_CULL_CCW
    //           | BGFX_STATE_MSAA
       //| BGFX_STATE_PT_LINES
       ;
       
       // Render skybox into view hdrSkybox.
       bgfx::setTexture(0, s_texCube, m_uffizi);
       bgfx::setState(_state);
       bgfx::setUniform(u_mtx, mtx);
       screen_space_quad( (float)m_width, (float)m_height, true);
       bgfx::submit(viewId, m_skyProgram);
    }



    void Sky::screen_space_quad(float _textureWidth, float _textureHeight, bool _originBottomLeft, float _width , float _height )
    {
       if (3 == bgfx::getAvailTransientVertexBuffer(3, ms_sky_layout) )
       {
           bgfx::TransientVertexBuffer vb;
           bgfx::allocTransientVertexBuffer(&vb, 3, ms_sky_layout);
           SkyPosColorTexCoord0Vertex* vertex = (SkyPosColorTexCoord0Vertex*)vb.data;

           const float zz = 0.0f;

           const float minx = -_width;
           const float maxx =  _width;
           const float miny = 0.0f;
           const float maxy = _height*2.0f;

           const float texelHalfW = s_texelHalf/_textureWidth;
           const float texelHalfH = s_texelHalf/_textureHeight;
           const float minu = -1.0f + texelHalfW;
           const float maxu =  1.0f + texelHalfW;

           float minv = texelHalfH;
           float maxv = 2.0f + texelHalfH;

           if (_originBottomLeft)
           {
               float temp = minv;
               minv = maxv;
               maxv = temp;

               minv -= 1.0f;
               maxv -= 1.0f;
           }

           vertex[0].m_x = minx;
           vertex[0].m_y = miny;
           vertex[0].m_z = zz;
           vertex[0].m_rgba = 0xffffffff;
           vertex[0].m_u = minu;
           vertex[0].m_v = minv;

           vertex[1].m_x = maxx;
           vertex[1].m_y = miny;
           vertex[1].m_z = zz;
           vertex[1].m_rgba = 0xffffffff;
           vertex[1].m_u = maxu;
           vertex[1].m_v = minv;

           vertex[2].m_x = maxx;
           vertex[2].m_y = maxy;
           vertex[2].m_z = zz;
           vertex[2].m_rgba = 0xffffffff;
           vertex[2].m_u = maxu;
           vertex[2].m_v = maxv;

           bgfx::setVertexBuffer(0, &vb);
       }
    }

}
