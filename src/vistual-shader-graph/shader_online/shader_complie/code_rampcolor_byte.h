/*
* Copyright 2021-2021 Zhouwei. All rights reserved.
* License: https://github.com/zwluoqi/mobile-visual-shader-editor#license-bsd-2-clause
*/

#pragma once
#include <cassert>
#include <cstddef>
#include <vector>


#include "vector.h"
#include "shader_def.h"
#include "slot.h"
#include "shared_state.h"

namespace csg {

	
	/// <summary>
	/// 将vector curve转为贴图数据
	/// </summary>
	/// <param name="mat"></param>
	/// <param name="cumap"></param>
	/// <param name="accessLayer"></param>
	/// <returns></returns>
    int SetVectorCurveSlotValue(const std::shared_ptr<cse::SharedState> shareState, const VectorCurveSlotValue* cumap,int accessLayer,csc::Float3* range = nullptr,csc::Float4* extx= nullptr,csc::Float4* exty= nullptr,csc::Float4* extz= nullptr);



    /// <summary>
    /// 将rgb curve转为贴图数据
    /// </summary>
    /// <param name="mat"></param>
    /// <param name="cumap"></param>
    /// <param name="accessLayer"></param>
    /// <returns></returns>
    int SetRGBCurveSlotValue(const std::shared_ptr<cse::SharedState> shareState, const RGBCurveSlotValue* cumap,int accessLayer,csc::Float4* range = nullptr,csc::Float4* extx= nullptr,csc::Float4* exty= nullptr,csc::Float4* extz= nullptr,csc::Float4* extw= nullptr);



	/// <summary>
	/// 将color ramp转为贴图数据
	/// </summary>
	/// <param name="mat"></param>
	/// <param name="cumap"></param>
	/// <param name="accessLayer"></param>
	/// <returns></returns>
    int SetColorRampSlotValue(const std::shared_ptr<cse::SharedState> shareState,const ColorRampSlotValue* cumap,int accessLayer) ;


	
    //将wave数据转为贴图
    int SetWaveLength(const std::shared_ptr<cse::SharedState> shareState, int accessLayer);
}
