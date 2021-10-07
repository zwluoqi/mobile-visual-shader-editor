#pragma once

/**
 * @file
 * @brief Defines SharedState. 
 */

#include <atomic>
#include <mutex>
#include <string>
#include <map>
#include <vector>
#include <bx/file.h>
#include "shader_def.h"
#include "vector.h"

/**
 * @brief Thread-safe class to allow the main window thread to send out a serialized graph to another thread
 */
namespace cse {
	class SharedState {
	public:
		bool output_updated();

		bool input_reload();
		void set_input_reload();

		std::string get_output_graph();
		void set_output_graph(const std::string& new_graph);

		void set_output_code(const std::string& nodeContent,const std::string& vertexCode, const std::string& fragmentCode, const std::string& vardefineCode,const void* data,const uint32_t size);
		void get_output_code(std::string& new_graph,std::string& vertexCode,std::string& fragmentCode, std::string& vardefineCode,void*& outputData,uint32_t& outputSize);

		void request_stop() { return stop.store(true); }
		bool should_stop() { return stop.load(); }
        
        
        void setStatePath(const bx::FilePath& bxFilePath,bool absPath){

            auto fileNam = bxFilePath.getBaseName();
            auto baseNamePath = bx::FilePath(fileNam);
            curGraphName = baseNamePath.getCPtr();
            
            auto dirNam = bxFilePath.getPath();
            auto dirPath = bx::FilePath(dirNam);
            curGraphDir = dirPath.getCPtr();
            
            abslotePath = absPath;
        }
        
        void new_input_graph(){
            curGraphName = "default";
            curGraphDir = "src_sources";
            abslotePath = false;
            
        }
        
        std::string getVertexName(){
            return curGraphName + ("_vertex");
        }
        std::string getFragName(){
            return curGraphName + ("_frag");
        }
        std::string getVertexFilePathWithPrefix();
        std::string getFragFilePathWithPrefix();
        std::string getVaringFilePathWithPrefix();
        std::string getNodeDataFilePathWithPrefix();
        bool getAbs(){return abslotePath;}
        
        uint32_t getStreamDataSize();
        
        void push_slot_val_change(uint64_t uniformId,float val);
        void push_slot_val_change(uint64_t uniformId,csc::Float2 val);
        void push_slot_val_change(uint64_t uniformId,csc::Float3 val);
        void push_slot_val_change(uint64_t uniformId,csc::Float4 val);
        void push_slot_sampler_change(uint64_t uniformId,bgfx::TextureHandle textureData);
        void holdWaitInputFile(){waitInputFile = true;}
        void releaseWaitInputFile(){waitInputFile = false;}
        bool getWaitInputFile(){return waitInputFile;}
        
        void push_slot_enum_change();
        
        bool uniform_updated();
        bool get_uniform_updated(void*& outputData,uint32_t& size);
        

        bgfx::TextureHandle AddTextureHandler(const char* filePath,bgfx::TextureInfo** textureInfo = nullptr);
        bgfx::TextureHandle GetRampTextureHandler();
        uint64_t GetRampTextureUniformId();
        void UpdateLayerRampBuffer(uint32_t* source,int layer);
        
        uint16_t getSlotLayer(uint64_t slotId);
        void setSlotLayer(uint64_t slotId,uint16_t layer);
        
        void record_connect_nodes(std::vector<uint64_t> nodes);
        bool has_connect_nodes(uint64_t nodeId){return connect_nodes_map.count(nodeId)>0;}
	private:

		std::mutex input_mutex;
		bool _input_updated{ true };

		std::mutex output_mutex;
		std::string output_graph;
		std::string output_vertex;
		std::string output_fragment;
		std::string output_varing;
        void* streamUniformData = nullptr;
        uint32_t streamUniformSize;
        
		bool _output_updated{ false };

		std::atomic<bool> stop{ false };
        
        std::string curGraphName{"default"};
        std::string curGraphDir{"src_sources"};
        bool abslotePath{false};
        bool waitInputFile{false};
        
        csc::UniformChangeData uniformChangeData[64];
        std::map<uint64_t,uint8_t> uniformChangeIndex;
        uint8_t uniform_change_counter;
        std::mutex uniform_change_mutex;
        
        
        csc::MaterialShaderContext materialShaderContext;
        
        std::map<uint64_t,bool> connect_nodes_map;
	};
}
