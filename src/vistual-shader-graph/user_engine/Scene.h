/*
* Copyright 2021-2021 Zhouwei. All rights reserved.
* License: https://github.com/zwluoqi/mobile-visual-shader-editor#license-bsd-2-clause
*/

#pragma once

#include <vector>
#include <memory>
#include <map>

#include "common.h"
#include "bgfx_utils.h"
#include "camera.h"

#include "ShaderMaterialDef.h"
#include "MeshUtil.h"
#include "Object.h"
#include "Light.h"
#include "../shader_online/shader_core/shader_def.h"

namespace userengine {
    class Scene
    {
    public:
        Scene();
        
        void add(std::shared_ptr<userengine::Object> object) { objects.push_back(object); }
        void add(std::shared_ptr<userengine::Light> light) { lights.push_back(light); }
        
        void load_model(std::string& path);
        
        void reload_shader_program(const char* vs_file_name,const char* fs_file_name,void* shader_uniform_stream_data,uint32_t shader_uniform_stream_size);
        
        void reset_shader_uniform_data(void* shader_uniform_stream_data,uint32_t shader_uniform_stream_size);
        
        void draw_call(float deltaTime,int viewId);

        void destroy();

    private:

        void set_material_uniform_data();

        MeshUtil mesh_util;
        std::vector<std::shared_ptr<userengine::Object> > objects;
        std::vector<std::shared_ptr<userengine::Light> > lights;
        
        bgfx::ProgramHandle model_program_handle;
        bgfx::ProgramHandle light_program_handle;
        
        ShaderUniformHandle stream_shader_uniform_handle[32];
        uint8_t cur_stream_shader_uniform_counter = 0;
        
        std::mutex reload_shader_program_mutex;
        
        std::mutex uniform_change_mutex;
        std::map<uint64_t,ShaderUniformHandle*> uniform_handle_map;
    };
}
