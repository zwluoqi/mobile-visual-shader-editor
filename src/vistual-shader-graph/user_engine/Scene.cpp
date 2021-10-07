/*
* Copyright 2021-2021 Zhouwei. All rights reserved.
* License: https://github.com/zwluoqi/mobile-visual-shader-editor#license-bsd-2-clause
*/

#include <vector>
#include <memory>
#include <map>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>


#include "common.h"
#include "bgfx_utils.h"
#include "camera.h"

#include "ShaderMaterialDef.h"
#include "MeshUtil.h"
#include "Object.h"
#include "Light.h"

#include "Sky.h"
#include "vsg_global.h"

#include "Scene.h"

namespace userengine {
    
    Scene::Scene(){
        model_program_handle = vsg::load_shader_program("vs_cubes", "fs_cubes");
        light_program_handle = vsg::load_shader_program("vs_cubes", "fs_cubes");
        auto light = userengine::Light{userengine::Vector4(0.5,-0.5,-0.5,1.0), userengine::Vector4(1,1,1,1)};
        add(std::make_shared<userengine::Light>(light));
    }

    void Scene::load_model(std::string& path){
        auto mesh_filters = mesh_util.load_mesh_filters(path);
        for (auto& mesh_filter : mesh_filters) {
            add(mesh_filter);
        }
    }

    void Scene::reload_shader_program(const char* vs_file_name,const char* fs_file_name,void* shader_uniform_stream_data,uint32_t shader_uniform_stream_size){
        std::lock_guard<std::mutex> lock(reload_shader_program_mutex);//其他线程要求重新编译shader及重置uniform数据
        
        bgfx::destroy(model_program_handle);
        model_program_handle = vsg::load_shader_program(vs_file_name, fs_file_name);
        std::vector<csc::UniformData> uniformDatas(static_cast<csc::UniformData*>(shader_uniform_stream_data), static_cast<csc::UniformData*>(shader_uniform_stream_data) + shader_uniform_stream_size / sizeof(csc::UniformData));
        uniform_handle_map.clear();
        cur_stream_shader_uniform_counter = 0;
        for (const auto& this_point  : uniformDatas) {
            auto bgfxUniformType = s_shaderRemap[this_point.uniformType];
            ShaderUniformHandle* vec4Uniform = &stream_shader_uniform_handle[cur_stream_shader_uniform_counter];
            if(bgfx::isValid(vec4Uniform->uniformHandle)){
                bgfx::destroy(vec4Uniform->uniformHandle);
            }
            bgfx::UniformHandle u_handle = bgfx::createUniform(this_point.uniformName,bgfxUniformType);
            vec4Uniform->_type = bgfxUniformType;
            vec4Uniform->uniformHandle = u_handle;
            cur_stream_shader_uniform_counter++;
            if(this_point.uniformId!=0){//可以变化的参数
                uniform_handle_map[this_point.uniformId] = vec4Uniform;
            }
            
            if(bgfxUniformType == bgfx::UniformType::Vec4){
                vec4Uniform->value_union = Vec4UniformHandle{this_point.uniformUnionData.defaultValue};
            }
            else if(bgfxUniformType == bgfx::UniformType::Sampler ){
                vec4Uniform->value_union = SamplerUniformHandle{this_point.uniformUnionData.defaultTextureData.handle,this_point.uniformUnionData.defaultTextureData.samplerHandleIndex};
            }
            else{
                assert(false);
            }
        }
    }
    
    void Scene::reset_shader_uniform_data(void* shader_uniform_stream_data,uint32_t shader_uniform_stream_size){
        std::lock_guard<std::mutex> lock(uniform_change_mutex);//其他线程要求重置uniform数据
        csc::UniformChangeData* uniformChangeDatas = static_cast<csc::UniformChangeData*>(shader_uniform_stream_data);
        uint32_t arraySize = shader_uniform_stream_size/sizeof(csc::UniformChangeData);
        uint64_t uniformId;
        
        for(uint32_t i=0;i<arraySize;i++){
            uniformId = uniformChangeDatas[i].uniformId;
            if(uniformId ==0){
                continue;
            }
            if(uniform_handle_map.count(uniformId)){
                ShaderUniformHandle* handle = uniform_handle_map[uniformId];
                if(handle->_type == bgfx::UniformType::Vec4){
                    handle->value_union.vec4_value.vec = uniformChangeDatas[i].uniformUnionData.defaultValue;
                }else if(handle->_type == bgfx::UniformType::Sampler){
                    handle->value_union.sampler_value.tex = uniformChangeDatas[i].uniformUnionData.defaultTextureData.handle;
                }else{
                    assert(false);
                }
            }
        }
    }
    
    void Scene::draw_call(float deltaTime,int viewId){
        for (int i = 0; i < objects.size(); i++) {
            for(int l=0;l<lights.size();l++){
                set_material_uniform_data();
                lights[l]->draw_call(deltaTime,viewId,light_program_handle);
                objects[i]->draw_call(viewId,model_program_handle);
            }
        }
    }

    void Scene::destroy(){
        bgfx::destroy(model_program_handle);
        for (int i = 0; i < objects.size(); i++) {
            objects[i]->destroy( );
        }
        for (int i = 0; i < lights.size(); i++) {
            lights[i]->destroy( );
        }
    }


    void Scene::set_material_uniform_data(){
        for(uint8_t i=0;i<cur_stream_shader_uniform_counter;i++) {
            auto& this_point = stream_shader_uniform_handle[i];
            if(this_point._type == bgfx::UniformType::Sampler){
                bgfx::setTexture(this_point.value_union.sampler_value.texIndex,this_point.uniformHandle, this_point.value_union.sampler_value.tex);
            }else if(this_point._type == bgfx::UniformType::Vec4){
                bgfx::setUniform(this_point.uniformHandle, &this_point.value_union.vec4_value.vec);
            }else{
                assert(false);
            }
        }
    }
        
}
