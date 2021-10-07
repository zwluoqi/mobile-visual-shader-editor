/*
* Copyright 2021-2021 Zhouwei. All rights reserved.
* License: https://github.com/zwluoqi/mobile-visual-shader-editor#license-bsd-2-clause
*/
#pragma once

#include <bx/uint32_t.h>
#include <bx/thread.h>
#include <bx/os.h>
#include <stdio.h>
#include <string.h>

#include "bgfx_utils.h"
#include "imgui/imgui.h"

#include "shaderc/shaderc.h"
#include "common.h"

#include "imgui/imgui.h"
#include "Scene.h"
#include "Sky.h"


#include "main_window.h"
#include "shared_state.h"

namespace userengine {

    enum HoverWindow{
        None,
        ModelViewTitle,
        ModelViewInner,
    };

    struct ShaderComplieThreadData{
        std::shared_ptr<userengine::Scene> scene;
        std::shared_ptr<cse::SharedState> shared_state;
    };


    struct UEAppContext
    {
        int64_t m_timeOffset;
        uint32_t m_width;
        uint32_t m_height;
        std::shared_ptr<userengine::Scene> scene;
        std::shared_ptr<userengine::Sky> sky;
        std::shared_ptr<cse::SharedState> shared_state;
        std::shared_ptr<cse::MainWindow> main_window;
        float curTime;
        float deltaTime;
        float lastTime;
        entry::MouseState m_mouseState;//手指状态
        ImVec2 accLeftMouseMove;//单手指累计移动,z为滑动累计
        float accScroll;
        ImVec2 accRightMouseMove;//双手指累计移动,z为滑动累计
        ImVec2 mousePos;//手指位置
        float curScroll;
        bool mouseOver = true;//手指抬起
        bool leftMouseDrag = false;//单指窗口拖拽交互
        bool rightMouseDrag = false;//双指窗口拖拽交互
        bool interationMouseScroll = false;//窗口缩放交互
        HoverWindow hoverWindowId;//当前手指按压窗口
        ShaderComplieThreadData m_mte;//编译线程参数
        bx::Thread shader_complie_thread;//shader编译线程
        
    };

    static std::shared_ptr<UEAppContext> ue_ctx = NULL;
    const uint16_t ClearViewID = 0;
    const uint16_t ImguiViewID = 128;
    const uint16_t SkyViewID = 159;
    const uint16_t SceneViewID = 160;

    std::shared_ptr<UEAppContext> init();
    void add_scene_model(std::string& path);

    void run_scene_3dview(ImVec2 startPos, ImVec2 startSize);
    void run_scene_ui(ImVec2& startPos, ImVec2& startSize);
    void run_shader_editor();
    void run_frame_time();

    entry::MouseState* get_mouse_state();
}
