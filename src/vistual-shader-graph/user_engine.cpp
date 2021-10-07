/*
* Copyright 2021-2021 Zhouwei. All rights reserved.
* License: https://github.com/zwluoqi/mobile-visual-shader-editor#license-bsd-2-clause
*/

#include "user_engine.h"
#include "../common/entry/input.h"
#include "vsg_global.h"

namespace userengine {



    int32_t shader_complie_thread_call(bx::Thread* _thread, void* _userData) {
        BX_UNUSED(_thread);

        ShaderComplieThreadData* self = (ShaderComplieThreadData*)_userData;
        
        std::shared_ptr<userengine::Scene> scene = self->scene;
        const std::shared_ptr<cse::SharedState> shared_state = self->shared_state;
        
        uint32_t shaderUniformStreamSize = 0;
        void* shaderUniformStreamData = nullptr;
        
        uint32_t uniformChangeStreamSize = 0;
        void* uniformChangeStreamData = nullptr;
        while (true) {
            if (shared_state->output_updated()) {
                float startCTime = vsg::get_cur_time();


                std::string graphContent;
                std::string fragment;
                std::string vertex;
                std::string vardefine; shared_state->get_output_code(graphContent,vertex,fragment,vardefine,shaderUniformStreamData,shaderUniformStreamSize);
                
                bool abs = shared_state->getAbs();

                if(graphContent.size()>0){
                    auto filePath = shared_state->getNodeDataFilePathWithPrefix();
                    vsg::write_file(filePath.c_str(), (void*)graphContent.c_str(), (uint32_t)graphContent.length(),abs);
                }
                
                auto vertexfilePath = shared_state->getVertexFilePathWithPrefix();
                vsg::write_file(vertexfilePath.c_str(), (void*)vertex.c_str(), (uint32_t)vertex.length(),abs);
                auto fragfilePath = shared_state->getFragFilePathWithPrefix();
                vsg::write_file(fragfilePath.c_str(), (void*)fragment.c_str(), (uint32_t)fragment.length(),abs);
                auto varfilePath = shared_state->getVaringFilePathWithPrefix();
                vsg::write_file(varfilePath.c_str(), (void*)vardefine.c_str(), (uint32_t)vardefine.length(),abs);

                bool success1 = vsg::compile_shader(vertexfilePath.c_str() ,shared_state->getVertexName().c_str(),"vertex");
                bool success2 = vsg::compile_shader(fragfilePath.c_str(),shared_state->getFragName().c_str(),"fragment");

                if(success1 == 0 && success2 == 0){ scene->reload_shader_program(shared_state->getVertexName().c_str(),shared_state->getFragName().c_str(),shaderUniformStreamData,shaderUniformStreamSize);
                }else{
                    //tips error
                }
                float endCTime = vsg::get_cur_time();
                bx::printf("compileShader time:%f",(endCTime-startCTime));
            }
            
            if (shared_state->uniform_updated()) {
                shared_state->get_uniform_updated(uniformChangeStreamData,uniformChangeStreamSize);
                scene->reset_shader_uniform_data(uniformChangeStreamData,uniformChangeStreamSize);
            }
        }
        return 0;
    }

    void do_interaction_event(){
        auto m_mouseState = userengine::get_mouse_state();
        bool left_down = m_mouseState->m_buttons[entry::MouseButton::Left];
        if(left_down){
            if(ue_ctx->hoverWindowId == HoverWindow::ModelViewInner){
                ue_ctx->leftMouseDrag = true;
            }
            if(ue_ctx->mouseOver){
                ImGuiIO& io = ImGui::GetIO();
                ue_ctx->mouseOver = false;
                ue_ctx->mousePos.x = io.MousePos.x;
                ue_ctx->mousePos.y = io.MousePos.y;
            }
        }
        
        bool right_down = m_mouseState->m_buttons[entry::MouseButton::Right];
        if(right_down){
            if(ue_ctx->hoverWindowId == HoverWindow::ModelViewInner){
                ue_ctx->rightMouseDrag = true;
            }
            if(ue_ctx->mouseOver){
                ImGuiIO& io = ImGui::GetIO();
                ue_ctx->mouseOver = false;
                ue_ctx->mousePos.x = io.MousePos.x;
                ue_ctx->mousePos.y = io.MousePos.y;
            }
        }
        
        if(!left_down && !right_down){
            ue_ctx->leftMouseDrag = false;
            ue_ctx->rightMouseDrag = false;
            ue_ctx->mouseOver = true;
        }
        
        if(ue_ctx->hoverWindowId == HoverWindow::ModelViewInner){
            ue_ctx->interationMouseScroll = true;
        }
    }

    void process_model_view_rotation(){
        ImGuiIO& io = ImGui::GetIO();
        
        auto m_width = io.DisplaySize.x;
        auto m_height = io.DisplaySize.y;
        
        const float _deltaTime = ue_ctx->deltaTime;
        const float m_moveSpeed = 240.0f;

        auto mouseDeltax = io.MousePos.x - ue_ctx->mousePos.x;
        auto mouseDeltay = io.MousePos.y - ue_ctx->mousePos.y;
        float deltaMovex = mouseDeltax/m_width;
        float deltaMovey = mouseDeltay/m_height;
        ue_ctx->accLeftMouseMove.x += deltaMovex* _deltaTime * m_moveSpeed;
        ue_ctx->accLeftMouseMove.y += deltaMovey* _deltaTime * m_moveSpeed;
        ue_ctx->mousePos.x = io.MousePos.x;
        ue_ctx->mousePos.y = io.MousePos.y;
    }

    void process_model_view_location(){
        ImGuiIO& io = ImGui::GetIO();
        
        auto m_width = io.DisplaySize.x;
        auto m_height = io.DisplaySize.y;
        
        const float _deltaTime = ue_ctx->deltaTime;
        const float m_moveSpeed = 480.0f;

        auto mouseDeltax = io.MousePos.x - ue_ctx->mousePos.x;
        auto mouseDeltay = io.MousePos.y - ue_ctx->mousePos.y;
        float deltaMovex = mouseDeltax/m_width;
        float deltaMovey = mouseDeltay/m_height;
        ue_ctx->accRightMouseMove.x += deltaMovex* _deltaTime * m_moveSpeed;
        ue_ctx->accRightMouseMove.y += deltaMovey* _deltaTime * m_moveSpeed;
        ue_ctx->mousePos.x = io.MousePos.x;
        ue_ctx->mousePos.y = io.MousePos.y;
    }

    void process_model_view_scroll(){
        ImGuiIO& io = ImGui::GetIO();
        auto m_height = io.DisplaySize.y;
        const float _deltaTime = ue_ctx->deltaTime;
        const float m_moveSpeed = 240.0f;
        
        auto m_mouseState = get_mouse_state();
        auto delta_z = m_mouseState->m_mz - ue_ctx->curScroll;
        ue_ctx->curScroll = m_mouseState->m_mz;
        ue_ctx->accScroll += delta_z / m_height * _deltaTime * m_moveSpeed;
    }

    void run_scene_3dview(ImVec2 startPos, ImVec2 startSize)
    {
        
        do_interaction_event();
        //roation camera
        if(ue_ctx->leftMouseDrag){
            process_model_view_rotation();
        }
        //move camera
        if(ue_ctx->rightMouseDrag){
           process_model_view_location();
        }
        if(ue_ctx->interationMouseScroll){
            process_model_view_scroll();
        }
        
        float roation_matrix[16];
        bx::mtxRotateXY(roation_matrix
                       , -userengine::ue_ctx->accLeftMouseMove.y
                       , -userengine::ue_ctx->accLeftMouseMove.x
                       );
//        float localtion_matrix[16];
//        bx::mtxTranslate(localtion_matrix, userengine::ue_ctx->accRightMouseMove.x, userengine::ue_ctx->accRightMouseMove.y, 0);
        
        
        //Sky viewport.
        bgfx::setViewRect(SkyViewID, startPos.x, startPos.y, uint16_t(startSize.x), uint16_t(startSize.y) );
        const bgfx::Caps* caps = bgfx::getCaps();
        float Skyproj[16];
        bx::mtxOrtho(Skyproj, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f, 100.0f, 0.0f, caps->homogeneousDepth);
        bgfx::setViewTransform(SkyViewID, NULL, Skyproj);
        ue_ctx->sky->draw_call(SkyViewID,roation_matrix,uint16_t(startSize.x), uint16_t(startSize.y));
        
       
        // Set view 0 default viewport.
        bgfx::setViewRect(SceneViewID, startPos.x, startPos.y, uint16_t(startSize.x), uint16_t(startSize.y) );
        const float _deltaTime = userengine::ue_ctx->deltaTime;
        float view[16];
        bx::Vec3 at  = { 0.0f, 1.0f,  0.0f };
        at.x += userengine::ue_ctx->accRightMouseMove.x;
        at.y += userengine::ue_ctx->accRightMouseMove.y;
        
        bx::Vec3 eye = { 0.0f, 1.0f, -2.5f };
        eye.z += userengine::ue_ctx->accScroll;
        eye.z = std::min(-0.1f, eye.z);
        
        
        eye.x += userengine::ue_ctx->accRightMouseMove.x;
        eye.y += userengine::ue_ctx->accRightMouseMove.y;
        
        bx::Vec3 tmp = bx::mul(eye, roation_matrix);
        bx::mtxLookAt(view, tmp, at);
        float proj[16];
        bx::mtxProj(proj, 60.0f, float(startSize.x)/float(startSize.y), 0.1f, 100.0f, bgfx::getCaps()->homogeneousDepth);
        bgfx::setViewTransform(SceneViewID, view, proj);
        ue_ctx->scene->draw_call(_deltaTime,SceneViewID);
    }
     
    void run_shader_editor(){

        if (ue_ctx->shared_state->input_reload()) {
            uint32_t size = 0;
            auto abs =ue_ctx->shared_state->getAbs();
            auto filePath = ue_ctx->shared_state->getNodeDataFilePathWithPrefix();
            char* data = (char*)vsg::load_file(filePath.c_str(), &size,abs);
            ue_ctx->main_window->load_graph(data);
        }
        
        ue_ctx->main_window->event_loop_iteration();
    }


    void run_scene_ui(ImVec2& startPos, ImVec2& startSize)
    {
        ImGuiIO& io = ImGui::GetIO();
        auto m_width =io.DisplaySize.x;
        
        startPos = ImVec2(m_width - m_width / 5.0f - 25.0f, 25.0f);
        startSize = ImVec2(m_width / 5.0f, m_width / 5.0f);
        ImGui::SetNextWindowPos(
              startPos
            , ImGuiCond_FirstUseEver
            );
        ImGui::SetNextWindowSize(
              startSize
            , ImGuiCond_FirstUseEver
            );
        ImGuiWindowFlags imGuiWindowFlags = 0;
        if(ue_ctx->hoverWindowId != HoverWindow::ModelViewTitle){
            imGuiWindowFlags |= ImGuiWindowFlags_NoMove;
        }

        if(ImGui::Begin("Model"
            , NULL
            , imGuiWindowFlags|ImGuiWindowFlags_NoCollapse
                        )){
            auto curStartPos = ImGui::GetWindowPos();
            if(curStartPos.y < startPos.y){
                curStartPos.y = startPos.y;
                ImGui::SetWindowPos("Model", curStartPos);
            }
            if(curStartPos.x > startPos.x){
                curStartPos.x = startPos.x;
                ImGui::SetWindowPos("Model", curStartPos);
            }
            
            if (ImGui::IsWindowHovered(ImGuiHoveredFlags_RootWindow)) {
                ue_ctx->hoverWindowId = HoverWindow::ModelViewTitle;
            }else{
                ue_ctx->hoverWindowId = HoverWindow::None;
            }
            
            if(ImGui::BeginChild("ModelInner")){
                if (ImGui::IsWindowHovered(ImGuiHoveredFlags_ChildWindows)) {
                    ue_ctx->hoverWindowId = HoverWindow::ModelViewInner;
                }
                startPos = ImGui::GetWindowPos();
                startPos.x +=-8;
                startPos.y +=-8;
                startSize = ImGui::GetWindowSize();
                startSize.x += 16;
                startSize.y += 16;
            }
            ImGui::EndChild();
        }
        ImGui::End();
    }


    std::shared_ptr<UEAppContext> init(){
        ue_ctx = std::make_shared<UEAppContext>() ;
        
        ue_ctx->m_timeOffset = bx::getHPCounter();
        ue_ctx->lastTime = ((bx::getHPCounter()-ue_ctx->m_timeOffset)/double(bx::getHPFrequency() ) );
        ue_ctx->curTime = ue_ctx->lastTime;
        ue_ctx->deltaTime = 0.033;

        ue_ctx->shared_state =  std::make_shared<cse::SharedState>() ;
        ue_ctx->main_window =  std::make_shared<cse::MainWindow>(ue_ctx->shared_state) ;

        ue_ctx->sky = std::make_shared<userengine::Sky>();
        ue_ctx->scene = std::make_shared<userengine::Scene>();
        
        ue_ctx->m_mte.scene =ue_ctx->scene;
        ue_ctx->m_mte.shared_state =ue_ctx->shared_state;
        ue_ctx->shader_complie_thread.init(shader_complie_thread_call, &ue_ctx->m_mte);

        return ue_ctx;
    }


    void add_scene_model(std::string& path){
        ue_ctx->scene->load_model(path);
    }


    void run_frame_time(){
        ue_ctx->curTime = ((bx::getHPCounter()-ue_ctx->m_timeOffset)/double(bx::getHPFrequency() ) );
        ue_ctx->deltaTime = ue_ctx->curTime - ue_ctx->lastTime;
        ue_ctx->lastTime = ue_ctx->curTime;
    }

    entry::MouseState* get_mouse_state(){
        return &ue_ctx->m_mouseState;
    }

}
