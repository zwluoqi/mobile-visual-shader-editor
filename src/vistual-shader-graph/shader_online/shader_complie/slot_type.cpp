/*
 * Copyright 2021-2021 Zhouwei. All rights reserved.
 * License: https://github.com/zwluoqi/mobile-visual-shader-editor#license-bsd-2-clause
 */

#include "slot.h"

#include <cassert>
#include <cmath>

#include <boost/algorithm/clamp.hpp>
#include <boost/optional.hpp>

#include "../shader_core/config.h"
#include "../shader_core/rect.h"
#include "../shader_core/vector.h"




const char* csg::Slot::type_name_str(csg::SlotType slot_type)
{
    switch (slot_type) {
    case csg::SlotType::BOOL:
        return "Boolean";
    case csg::SlotType::COLOR:
        return "Color";
    case csg::SlotType::CLOSURE:
        return "Closure";
    case csg::SlotType::FLOAT:
        return "Float";
    case csg::SlotType::INT:
        return "Integer";
    case csg::SlotType::VECTOR:
        return "Vector";
    case csg::SlotType::ENUM:
        return "Enumeration";
    case csg::SlotType::CURVE_RGB:
        return "Curves_RGB_";
    case csg::SlotType::CURVE_VECTOR:
        return "Curves_Vector_";
    case csg::SlotType::COLOR_RAMP:
        return "Color_Ramp_";
    case csg::SlotType::IMAGE:
        return "Image";
    default:
        return "[ERROR]";
    }
}


bool csg::Slot::type_match(csg::SlotType slot_type_a,csg::SlotType slot_type_b){
    if ((slot_type_a == csg::SlotType::BOOL
       || slot_type_a ==  csg::SlotType::COLOR
       || slot_type_a ==  csg::SlotType::FLOAT
       || slot_type_a ==  csg::SlotType::INT
       || slot_type_a ==  csg::SlotType::VECTOR
         )){
        return (
        slot_type_b == csg::SlotType::BOOL
        || slot_type_b ==  csg::SlotType::COLOR
        || slot_type_b ==  csg::SlotType::FLOAT
        || slot_type_b ==  csg::SlotType::INT
        || slot_type_b ==  csg::SlotType::VECTOR
                );
    }else{
        return slot_type_a == slot_type_b;
    }
}
