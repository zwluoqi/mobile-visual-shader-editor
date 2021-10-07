/*
* Copyright 2021-2021 Zhouwei. All rights reserved.
* License: https://github.com/zwluoqi/mobile-visual-shader-editor#license-bsd-2-clause
*/

#pragma once


#include "BaseDefine.h"
#include <bgfx/bgfx.h>

namespace userengine {

    struct Texture {
        bgfx::TextureHandle m_textureColor;
        std::string type;
        std::string path;
        bgfx::UniformHandle s_texColor;
    };



    // Structure: Mesh
    //
    // Description: A Simple Mesh Object that holds
    //    a name, a vertex list, and an index list
    struct Mesh
    {
        // Variable Set Constructor
        Mesh(std::vector<Vertex>& _Vertices, std::vector<uint16_t>& _Indices, std::vector<Texture>& _textures)
        {
            Vertices = _Vertices;
            Indices = _Indices;
            textures = _textures;

            this->ms_layout
                .begin()
                .add(bgfx::Attrib::Position, 3, bgfx::AttribType::Float)
                .add(bgfx::Attrib::Color0, 4, bgfx::AttribType::Uint8, true)
                .add(bgfx::Attrib::Normal, 3, bgfx::AttribType::Float, true)
                .add(bgfx::Attrib::TexCoord0, 2, bgfx::AttribType::Float, true)
                .add(bgfx::Attrib::Tangent, 4, bgfx::AttribType::Float, true)
                .end();

            int verNum = (int)this->Vertices.size();
            this->m_vbh = bgfx::createVertexBuffer(
                // Static data can be passed with bgfx::makeRef
                bgfx::makeRef(&(this->Vertices[0]), sizeof(Vertex) * verNum)
                , this->ms_layout
            );
            int IndicNum = (int)this->Indices.size();
            this->m_ibh = bgfx::createIndexBuffer(
                bgfx::makeRef(&(this->Indices[0]), sizeof(uint16_t) * IndicNum)
            );
        }
        // Mesh Name
        std::string MeshName;
        // Vertex List
        std::vector<Vertex> Vertices;
        // Index List
        std::vector<uint16_t> Indices;

        std::vector<Texture> textures;

        bgfx::VertexBufferHandle m_vbh;
        bgfx::IndexBufferHandle m_ibh;
        bgfx::VertexLayout ms_layout;

        void destroy(){
            bgfx::destroy(m_vbh);
            bgfx::destroy(m_ibh);
        }
    };
}
