/*
* Copyright 2021-2021 Zhouwei. All rights reserved.
* License: https://github.com/zwluoqi/mobile-visual-shader-editor#license-bsd-2-clause
*/

#pragma once

//#include "Vector.hpp"
//#include "global.hpp"
//#include "bgfx_utils.h"
#include "bgfx_utils.h"
#include "BaseDefine.h"


namespace userengine {


    class Object
    {
    protected:
        Material m_material;
    public:
        Object(Material _mat):m_material(_mat)
        {}

        virtual ~Object() = default;

        virtual bool draw_call(bgfx::ViewId _id, bgfx::ProgramHandle m_program, uint64_t _state = BGFX_STATE_MASK) const = 0;
        virtual void destroy() const = 0;
    };

}
