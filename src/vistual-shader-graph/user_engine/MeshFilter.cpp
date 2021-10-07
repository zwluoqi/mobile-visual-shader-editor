/*
* Copyright 2021-2021 Zhouwei. All rights reserved.
* License: https://github.com/zwluoqi/mobile-visual-shader-editor#license-bsd-2-clause
*/

#include <bgfx/bgfx.h>
#include <bx/float4x4_t.h>

#include "MeshFilter.h"

namespace userengine {

    void MeshFilter::destroy() const {
        mesh->destroy();
    }

    bool MeshFilter::draw_call(bgfx::ViewId _id, bgfx::ProgramHandle m_program,uint64_t _state) const{
        
        bx::float4x4_t m_cache_model;
        bx::float4x4_t m_cache_inv_model;
    //            float mtx[16];
        bx::mtxTranslate((float *)m_cache_inv_model.col, 0, 1.0, 0);
    //            float mtx2[16];
        bx::mtxRotateY((float *)m_cache_model.col, 0);
        bx::mtxMul((float *)m_cache_model.col, (float *)m_cache_model.col, (float *)m_cache_inv_model.col);
        
        

        bx::float4x4_inverse(&m_cache_inv_model,&m_cache_model);
        
        bx::memCopy((void*)modelMatrixs, (void *)m_cache_model.col, sizeof(float)*16);
        bx::memCopy((void*)(modelMatrixs+16), (void *)m_cache_inv_model.col, sizeof(float)*16);
        
        bgfx::setTransform(modelMatrixs,2);

        
        if (BGFX_STATE_MASK == _state)
        {
            _state = 0
                | BGFX_STATE_WRITE_RGB
                | BGFX_STATE_WRITE_A
                | BGFX_STATE_WRITE_Z
                | BGFX_STATE_DEPTH_TEST_LESS
                | BGFX_STATE_CULL_CCW
                | BGFX_STATE_MSAA
                //| BGFX_STATE_PT_LINES
                ;
        }



        bgfx::setIndexBuffer(mesh->m_ibh);
        bgfx::setVertexBuffer(0, mesh->m_vbh);
        
        bgfx::setState(_state);

        bgfx::submit(_id, m_program);
        return true;
    }


}
