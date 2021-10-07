/*
 * Copyright 2021-2021 Zhouwei. All rights reserved.
 * License: https://github.com/zwluoqi/mobile-visual-shader-editor#license-bsd-2-clause
 */

#include "common.h"
#include "camera.h"
#include "bgfx_utils.h"
#include "imgui/imgui.h"
#include "vsg_global.h"
#include "user_engine.h"

namespace
{

class VSGEntry : public entry::AppI
{
public:

    VSGEntry(const char* _name, const char* _description, const char* _url)
        : entry::AppI(_name, _description, _url)
    {
    }

    void init(int32_t _argc, const char* const* _argv, uint32_t _width, uint32_t _height) override
    {

        Args args(_argc, _argv);

        m_debug  = BGFX_DEBUG_NONE;
        m_reset  = BGFX_RESET_VSYNC;

        bgfx::Init init;
        init.type     = args.m_type;
        init.vendorId = args.m_pciId;
        init.resolution.width  = _width;
        init.resolution.height = _height;
        init.resolution.reset  = m_reset;
        bgfx::init(init);

        // Enable debug text.
        bgfx::setDebug(m_debug);

        // Set view 0 clear state.
        bgfx::setViewClear(0
                , BGFX_CLEAR_COLOR|BGFX_CLEAR_DEPTH
                , 0x303030ff
                , 1.0f
                , 0
                );
        vsg::init();
        ue_ctx = userengine::init();
        

        std::string path = "meshes/spot/spot_triangulated_good.obj";
        userengine::add_scene_model(path);
        
        imguiCreate();
    }
    

    int shutdown() override
    {
        vsg::destroy();
        
        cameraDestroy();
        
        imguiDestroy();

        
        // Shutdown bgfx.
        bgfx::shutdown();

        return 0;
    }

    bool update() override
    {
        auto m_mouseState = userengine::get_mouse_state();
        uint32_t m_width;
        uint32_t m_height;
        if (!entry::processEvents(m_width, m_height, m_debug, m_reset, m_mouseState) )
        {
            ue_ctx->m_width = m_width;
            ue_ctx->m_height = m_height;
            userengine::run_frame_time();
            
            // Set view 0 default viewport.
            bgfx::setViewRect(userengine::ClearViewID, 0, 0, uint16_t(ue_ctx->m_width), uint16_t(ue_ctx->m_height) );

            // This dummy draw call is here to make sure that view 0 is cleared
            // if no other draw calls are submitted to view 0.
            bgfx::touch(userengine::ClearViewID);
            
            
            imguiBeginFrame(m_mouseState->m_mx
                ,  m_mouseState->m_my
                , (m_mouseState->m_buttons[entry::MouseButton::Left  ] ? IMGUI_MBUT_LEFT   : 0)
                | (m_mouseState->m_buttons[entry::MouseButton::Right ] ? IMGUI_MBUT_RIGHT  : 0)
                | (m_mouseState->m_buttons[entry::MouseButton::Middle] ? IMGUI_MBUT_MIDDLE : 0)
                ,  m_mouseState->m_mz
                , uint16_t(ue_ctx->m_width)
                , uint16_t(ue_ctx->m_height)
                ,-1,userengine::ImguiViewID);
            
            
            ImVec2 startPos;
            ImVec2 startSize;
            userengine::run_shader_editor();
            userengine::run_scene_ui(startPos,startSize);
            
            imguiEndFrame();
            
            userengine::run_scene_3dview(startPos,startSize);
            
            // Advance to next frame. Rendering thread will be kicked to
            // process submitted rendering primitives.
            bgfx::frame();

            return true;
        }

        return false;
    }


    uint32_t m_debug;
    uint32_t m_reset;

    
    bgfx::UniformHandle u_time;
    std::shared_ptr<userengine::UEAppContext> ue_ctx;
};

} // namespace

ENTRY_IMPLEMENT_MAIN(
      VSGEntry
    , "visual-shader-graph"
    , "VSG"
    , "https://github.com/zwluoqi/mobile-visual-shader-editor#license-bsd-2-clause"
    );
