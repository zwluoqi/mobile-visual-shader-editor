/*
* Copyright 2021-2021 Zhouwei. All rights reserved.
* License: https://github.com/zwluoqi/mobile-visual-shader-editor#license-bsd-2-clause
*/

#pragma once

#include "Object.h"
#include "Mesh.h"

namespace userengine {
    class MeshFilter : public Object
    {
    public:
        MeshFilter(std::shared_ptr<userengine::Mesh> _mesh,Material _material):userengine::Object(_material),mesh(_mesh){
            
        }
        
        void destroy() const override ;
        
        bool draw_call(bgfx::ViewId _id, bgfx::ProgramHandle m_program,uint64_t _state = BGFX_STATE_MASK) const override;
        
    private:
        std::shared_ptr<userengine::Mesh> mesh;
        float modelMatrixs[32];//model_matrix,inv_model_matrix
    };

}
