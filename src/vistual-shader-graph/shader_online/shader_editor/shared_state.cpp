#include "shared_state.h"
#include <sstream>

bool cse::SharedState::input_reload()
{
	std::lock_guard<std::mutex> lock(input_mutex);
	const bool result{ _input_updated };
    _input_updated = false;
	return result;
}

void cse::SharedState::set_input_reload()
{
    std::lock_guard<std::mutex> lock(input_mutex);
    _input_updated = true;
}


bool cse::SharedState::output_updated()
{
	std::lock_guard<std::mutex> lock(output_mutex);
	const bool result{ _output_updated };
	return result;
}

bool cse::SharedState::uniform_updated(){
    std::lock_guard<std::mutex> lock(uniform_change_mutex);
    const bool result = uniform_change_counter>0;
    return result;
}

bool cse::SharedState::get_uniform_updated(void*& outputData,uint32_t& size){
    std::lock_guard<std::mutex> lock(uniform_change_mutex);
    size = sizeof(csc::UniformChangeData)*uniform_change_counter;
    outputData = realloc(outputData, size);
    memcpy(outputData, uniformChangeData, size);
    uniform_change_counter = 0;
    uniformChangeIndex.clear();
    return true;
}



std::string cse::SharedState::get_output_graph()
{
	std::lock_guard<std::mutex> lock(output_mutex);
	const std::string result{ output_graph };
	_output_updated = false;
	return result;
}

void cse::SharedState::set_output_graph(const std::string& new_graph)
{
	std::lock_guard<std::mutex> lock(output_mutex);
	output_graph = new_graph;
	_output_updated = true;
}

void cse::SharedState::get_output_code(std::string& new_graph,std::string& vertexCode,std::string& fragmentCode, std::string& vardefineCode,void*& outputData,uint32_t& outputSize) {
	std::lock_guard<std::mutex> lock(output_mutex);
    new_graph = std::string{ output_graph };
	vertexCode = std::string{ output_vertex };
    fragmentCode = std::string{ output_fragment };
    vardefineCode = std::string{ output_varing };
    outputSize = streamUniformSize;
    outputData = realloc(outputData, streamUniformSize);
    memcpy(outputData, streamUniformData, streamUniformSize);
	_output_updated = false;
}

void cse::SharedState::set_output_code(const std::string& new_graph,const std::string& vertexCode, const std::string& fragmentCode, const std::string& vardefineCode,const void* data,const uint32_t size)
{
	std::lock_guard<std::mutex> lock(output_mutex);
    output_graph = new_graph;
	output_vertex = vertexCode;
	output_fragment = fragmentCode;
    output_varing = vardefineCode;
    streamUniformData = realloc(streamUniformData, size);
    memcpy(streamUniformData, data, size);
    streamUniformSize = size;
	_output_updated = true;
}

uint32_t cse::SharedState::getStreamDataSize(){
    std::lock_guard<std::mutex> lock(output_mutex);
    const uint32_t result{ streamUniformSize };
    return result;
}

std::string cse::SharedState::getVertexFilePathWithPrefix(){
    std::stringstream filePath;
    filePath<<curGraphDir<<"/"<<curGraphName<<"/"<<curGraphName<<"_vertex.sc";
    return filePath.str();
}
std::string cse::SharedState::getFragFilePathWithPrefix(){
    std::stringstream filePath;
    filePath<<curGraphDir<<"/"<<curGraphName<<"/"<<curGraphName<<"_frag.sc";
    return filePath.str();
}
std::string cse::SharedState::getVaringFilePathWithPrefix(){
   std::stringstream filePath;
   filePath<<curGraphDir<<"/"<<curGraphName<<"/"<<"varying.def.sc";
   return filePath.str();
}

std::string cse::SharedState::getNodeDataFilePathWithPrefix(){
    std::stringstream filePath;
    filePath<<curGraphDir<<"/"<<curGraphName<<".nodedata";
    return filePath.str();
}



void cse::SharedState::push_slot_val_change(uint64_t uniformId,float val){
    if(uniformId ==0){
        return;
    }
    csc::UniformChangeData data{uniformId,csc::Float4(val,0.0,0.0,0.0)};

    std::lock_guard<std::mutex> lock(uniform_change_mutex);
    if(uniformChangeIndex.count(uniformId)>0){
        auto uniformIndex = uniformChangeIndex.at(uniformId);
        uniformChangeData[uniformIndex] = data;
    }else{
        uniformChangeIndex[uniformId] = uniform_change_counter;
        uniformChangeData[uniform_change_counter++] = data;
    }
}
void cse::SharedState::push_slot_val_change(uint64_t uniformId,csc::Float2 val){
    if(uniformId ==0){
        return;
    }
    csc::UniformChangeData data{uniformId,csc::Float4(val.x,val.y,0.0,0.0)};
    std::lock_guard<std::mutex> lock(uniform_change_mutex);
    if(uniformChangeIndex.count(uniformId)>0){
        auto uniformIndex = uniformChangeIndex.at(uniformId);
        uniformChangeData[uniformIndex] = data;
    }else{
        uniformChangeIndex[uniformId] = uniform_change_counter;
        uniformChangeData[uniform_change_counter++] = data;
    }
}
void cse::SharedState::push_slot_val_change(uint64_t uniformId,csc::Float3 val){
    if(uniformId ==0){
        return;
    }
    csc::UniformChangeData data{uniformId,csc::Float4(val.x,val.y,val.z,0.0)};
    std::lock_guard<std::mutex> lock(uniform_change_mutex);
    if(uniformChangeIndex.count(uniformId)>0){
        auto uniformIndex = uniformChangeIndex.at(uniformId);
        uniformChangeData[uniformIndex] = data;
    }else{
        uniformChangeIndex[uniformId] = uniform_change_counter;
        uniformChangeData[uniform_change_counter++] = data;
    }
}
void cse::SharedState::push_slot_val_change(uint64_t uniformId,csc::Float4 val){
    if(uniformId ==0){
        return;
    }
    csc::UniformChangeData data{uniformId,val};
    std::lock_guard<std::mutex> lock(uniform_change_mutex);
    if(uniformChangeIndex.count(uniformId)>0){
        auto uniformIndex = uniformChangeIndex.at(uniformId);
        uniformChangeData[uniformIndex] = data;
    }else{
        uniformChangeIndex[uniformId] = uniform_change_counter;
        uniformChangeData[uniform_change_counter++] = data;
    }
}

void cse::SharedState::push_slot_sampler_change(uint64_t uniformId,bgfx::TextureHandle samplerUid){
    if(uniformId ==0){
        return;
    }
    csc::UniformChangeData data{uniformId,csc::TextureData{samplerUid}};

    std::lock_guard<std::mutex> lock(uniform_change_mutex);
    if(uniformChangeIndex.count(uniformId)>0){
        auto uniformIndex = uniformChangeIndex.at(uniformId);
        uniformChangeData[uniformIndex] = data;
    }else{
        uniformChangeIndex[uniformId] = uniform_change_counter;
        uniformChangeData[uniform_change_counter++] = data;
    }
}

void cse::SharedState::push_slot_enum_change(){
    
}

bgfx::TextureHandle cse::SharedState::AddTextureHandler(const char* filePth,bgfx::TextureInfo** textureInfo){
    return materialShaderContext.addTextureHandle(filePth,textureInfo);
}

bgfx::TextureHandle cse::SharedState::GetRampTextureHandler(){
    return materialShaderContext.GetRampTextureHandler();
}

uint64_t cse::SharedState::GetRampTextureUniformId(){
    return materialShaderContext.GetRampTextureUniformId();
}

void cse::SharedState::UpdateLayerRampBuffer(uint32_t* source,int layer){
    materialShaderContext.UpdateLayerRampBuffer(source,layer);
}


uint16_t cse::SharedState::getSlotLayer(uint64_t slotId){
    return materialShaderContext.GetSlotLayer(slotId);
}
void cse::SharedState::setSlotLayer(uint64_t slotId,uint16_t layer){
    materialShaderContext.SetSlotLayer(slotId, layer);
}

void cse::SharedState::record_connect_nodes(std::vector<uint64_t> nodes){
    connect_nodes_map.clear();
    for (auto nodeId : nodes) {
        connect_nodes_map[nodeId] = true;
    }
}
