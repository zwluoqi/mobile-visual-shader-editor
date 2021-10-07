/*
* Copyright 2021-2021 Zhouwei. All rights reserved.
* License: https://github.com/zwluoqi/mobile-visual-shader-editor#license-bsd-2-clause
*/

#pragma once

#include <string>
#include <algorithm>
#include <array>
#include <cassert>
#include <cstddef>
#include <cstring>
#include <iomanip>
#include <list>
#include <map>
#include <memory>
#include <sstream>
#include <utility>
#include <vector>
#include <boost/optional.hpp>
#include "../shader_core/shader_def.h"
#include "../shader_editor/shared_state.h"
#include "node_id.h"

using namespace csc;
//
//namespace cse {
//    class SharedState;
//}

namespace csg {
	class Graph;
    

    //
    //Some differences between bgfx’s shaderc flavor of GLSL and regular GLSL:
    //
    //No bool/int uniforms, all uniforms must be float.
    //
    //Attributes and varyings can be accessed only from main() function.
    //
    //Must use SAMPLER2D/3D/CUBE/etc. macros instead of sampler2D/3D/Cube/etc. tokens.
    //
    //Must use vec2/3/4_splat(<value>) instead of vec2/3/4(<value>).
    //
    //Must use mtxFromCols/mtxFromRows when constructing matrices in shaders.
    //
    //Must use mul(x, y) when multiplying vectors and matrices.
    //
    //Must use varying.def.sc to define input/output semantic and precission instead of using attribute/in and varying/in/out.

	

    static const char* pre_uniform_str="\n\
    //{\n\
    //    \"u_viewRect\",\n\
    //    \"u_viewTexel\",\n\
    //    \"u_view\",\n\
    //    \"u_invView\",\n\
    //    \"u_proj\",\n\
    //    \"u_invProj\",\n\
    //    \"u_viewProj\",\n\
    //    \"u_invViewProj\",\n\
    //    \"u_model\",array\n\
    //    \"u_modelView\",\n\
    //    \"u_modelViewProj\",\n\
    //    \"u_alphaRef4\",\n\
    //};\n";


    struct ShaderAttrRemap
    {
        csc::ShaderAttributeType::Enum m_attr;
        const char* m_name;
        uint8_t m_index;
    };

    static const ShaderAttrRemap s_shaderRemap[csc::ShaderAttributeType::Count + 1] =
    {
        { csc::ShaderAttributeType::Position,  "POSITION",     0 },
        { csc::ShaderAttributeType::Normal,    "NORMAL",       0 },
        { csc::ShaderAttributeType::Tangent,   "TANGENT",      0 },
        { csc::ShaderAttributeType::Bitangent, "BITANGENT",    0 },
        { csc::ShaderAttributeType::Color0,    "COLOR0",        0 },
        { csc::ShaderAttributeType::Color1,    "COLOR1",        1 },
        { csc::ShaderAttributeType::Color2,    "COLOR2",        2 },
        { csc::ShaderAttributeType::Color3,    "COLOR3",        3 },
        { csc::ShaderAttributeType::Indices,   "BLENDINDICES",  0 },
        { csc::ShaderAttributeType::Weight,    "BLENDWEIGHT",   0 },
        { csc::ShaderAttributeType::TexCoord0, "TEXCOORD0",     0 },
        { csc::ShaderAttributeType::TexCoord1, "TEXCOORD1",     1 },
        { csc::ShaderAttributeType::TexCoord2, "TEXCOORD2",     2 },
        { csc::ShaderAttributeType::TexCoord3, "TEXCOORD3",     3 },
        { csc::ShaderAttributeType::TexCoord4, "TEXCOORD4",     4 },
        { csc::ShaderAttributeType::TexCoord5, "TEXCOORD5",     5 },
        { csc::ShaderAttributeType::TexCoord6, "TEXCOORD6",     6 },
        { csc::ShaderAttributeType::TexCoord7, "TEXCOORD7",     7 },
        { csc::ShaderAttributeType::Count,     "",             0 },
    };

    static const char* s_uniforType[csc::UniformType::Count + 1] =
    {
        "SAMPLER2D",
        "end",
        "vec4",
        "mat3",
        "mat4",
        "vec3",
        "vec2",
        "float",
        "",
    };

    static const char* s_uniforVal[csc::UniformType::Count + 1] =
    {
        "0.0",
        "",
        "vec4(0.0,0.0,0.0,0.0)",
        "0.0",
        "0.0",
        "vec3(0.0,0.0,0.0)",
        "vec2(0.0,0.0)",
        "0.0",
        "",
    };


    struct UniformCodeData {
        csc::ShaderDataType::Enum dataType;//attribute,uniform,varying
        char uniformName[64];
        csc::UniformType::Enum uniformType;
        csc::ShaderAttributeType::Enum attributeType = csc::ShaderAttributeType::Count;
        char defaultVal[64];
        Float4 defaultValue;//mat3 mat4 暂时不支持
        uint64_t uniformId;
        char textFilePath[256];
    };

	struct FunData {
		std::string funName;
		std::string funContent;
	};


	class CodeGenerateData {
	private:
		std::map<std::string, bool> mainFragParamMap;
		std::map<std::string, FunData> fragmentFunMap;
		std::stringstream fragment_head_stream;
		std::stringstream vertex_head_stream;

		std::map<std::string, UniformCodeData> params;
        std::vector<UniformCodeData> varyingParams;
        std::vector<UniformCodeData> attributeParams;
        std::vector<UniformCodeData> uniformVertexParams;
        std::vector<UniformCodeData> uniformFragParams;
        
        std::stringstream varying_def_stream;
        
        //add info to process code
        std::map<csg::NodeId, int> processedNodes;
        //当前引用2Du图片的高度索引
        
	public:
		std::stringstream fragment_stream;
		std::stringstream vertex_stream;
        int current_layer;//每次重新生成ramp重置为0
        std::shared_ptr<csg::Graph> graph;
        std::shared_ptr<cse::SharedState> shareState;


		CodeGenerateData(std::shared_ptr<csg::Graph> _graph,std::shared_ptr<cse::SharedState> _shareState) {
            graph = _graph;
            shareState = _shareState;
			fragment_head_stream << "//--------\n";
			fragment_head_stream << "//this fragment code generate by zhouxingxing's visual shader scirpt app\n";
            fragment_head_stream<<pre_uniform_str;
			fragment_head_stream << "//Version 1.0\n";
			fragment_head_stream << "//--------\n";

			vertex_head_stream << "//--------\n";
			vertex_head_stream << "//this vertex code generate by zhouxingxing's visual shader scirpt app\n";
            vertex_head_stream<<pre_uniform_str;
			vertex_head_stream << "//Version 1.0\n";
			vertex_head_stream << "//--------\n";
            current_layer = 0;
		}

        bool hasProcessedNode(const csg::NodeId nodeId){
            return processedNodes.count(nodeId) >0;
        }
        void processedNode(const csg::NodeId nodeId){
            processedNodes[nodeId] = 0;
        }

		bool HasFun(std::string funName) {
			return fragmentFunMap.count(funName);
		}

		void AddFun(std::string funName, std::string funContent) {
			if (fragmentFunMap.count(funName)>0) {
				return;
			}
			FunData funData{ funName,funContent };
			fragmentFunMap[funData.funName] = funData;
		}

		bool HasMainParams(std::string mainParam) {
			return mainFragParamMap.count(mainParam);
		}

		void AddMainParams(std::string mainParam) {
			if (mainFragParamMap.count(mainParam) > 0) {
				return;
			}
			mainFragParamMap[mainParam] = true;
		}

		bool AddUniformParams(const UniformCodeData& uniformData) {
			if (params.count(uniformData.uniformName) > 0) {
				return false;
			}
			params[uniformData.uniformName] = uniformData;
            if(uniformData.dataType == ShaderDataType::attribute){
                attributeParams.push_back(uniformData);
            }
            else if(uniformData.dataType == ShaderDataType::varying){
                varyingParams.push_back(uniformData);
            }
            else if(uniformData.dataType == ShaderDataType::uniformFrag){
                uniformFragParams.push_back(uniformData);
            }
            else if(uniformData.dataType == ShaderDataType::uniformVertex){
                uniformVertexParams.push_back(uniformData);
            }
            return true;
		}
        
        const char * varDefineName(csc::ShaderAttributeType::Enum attribute)
        {
            int idx = static_cast<int>(attribute);
            return csg::s_shaderRemap[idx].m_name;
        }
        const char * varDefineName(int idx)
        {
            return csg::s_shaderRemap[idx].m_name;
        }
        
        std::string ExportVaring(){


            int texcoordIndex = 0;
            int idx = static_cast<int>(ShaderAttributeType::TexCoord0);
            for (const auto& uniformData : this->varyingParams) {
                if(uniformData.attributeType == ShaderAttributeType::Count){
                    this->varying_def_stream << s_uniforType[uniformData.uniformType] << " " << uniformData.uniformName << ":" << varDefineName(idx+texcoordIndex)<<"="<<s_uniforVal[uniformData.uniformType]<<";\n";
                    texcoordIndex++;
                }else{
                    this->varying_def_stream << s_uniforType[uniformData.uniformType] << " " << uniformData.uniformName << ":" << varDefineName(uniformData.attributeType)<<"="<<s_uniforVal[uniformData.uniformType]<<";\n";
                }
            }
            
            
            this->varying_def_stream << "\n";
            this->varying_def_stream << "\n";
            
            
            texcoordIndex = 0;
            for (const auto& uniformData : this->attributeParams) {
                if(uniformData.attributeType == ShaderAttributeType::Count){
                    this->varying_def_stream << s_uniforType[uniformData.uniformType] << " " << uniformData.uniformName << ":" << varDefineName(idx+texcoordIndex)<<";\n";
                    texcoordIndex++;
                }else{
                    this->varying_def_stream << s_uniforType[uniformData.uniformType] << " " << uniformData.uniformName << ":" << varDefineName(uniformData.attributeType)<<";\n";
                }
            }
            
            
            return this->varying_def_stream.str();
        }
        
		std::string ExportFragment() {
            this->fragment_head_stream << "\n";
            this->fragment_head_stream << "$input ";
            int i=0;
            for (const auto& uniformData : this->varyingParams) {
                this->fragment_head_stream << " " << uniformData.uniformName;
                i++;
                if(i<this->attributeParams.size()){
                   this->fragment_head_stream << ",";
                }
            }
            this->fragment_head_stream << " // in...\n";
            this->fragment_head_stream << "\n";
            this->fragment_head_stream << "#include \"../common/common.sh\"\n";
            this->fragment_head_stream << "#include \"../node/node.cginc\"\n";
            this->fragment_head_stream << "\n";
            
            u_int16_t samplerIndex = 0;
            for (auto& uniformData : this->uniformFragParams) {
                if(uniformData.uniformType == csc::UniformType::Sampler){
                    this->fragment_head_stream << "SAMPLER2D("  << uniformData.uniformName << ","<< (samplerIndex) <<");\n";
                    samplerIndex++;
                }else{
                    this->fragment_head_stream << "uniform " << s_uniforType[uniformData.uniformType] << " " << uniformData.uniformName << ";\n";
                }
            }
            this->fragment_head_stream << "\n";
            
//            for (const auto& uniformData : this->localParams) {
//                this->fragment_head_stream << "" << uniformData.valType << " " << uniformData.uniformName << ";\n";
//            }

			this->fragment_head_stream << "\n";
			this->fragment_head_stream << "\n";
			this->fragment_head_stream << "\n";
			this->fragment_head_stream << "\n";
			this->fragment_head_stream << "//----------------start generate api----------------\n";

			for (const auto& funData : this->fragmentFunMap) {
				this->fragment_head_stream << funData.second.funContent;
			}
			this->fragment_head_stream << "//----------------end generate api----------------\n";
			this->fragment_head_stream << "\n";
			this->fragment_head_stream << "\n";
			this->fragment_head_stream << "\n";
			this->fragment_head_stream << "\n";
			

			this->fragment_head_stream << "void main(){\n";
			this->fragment_head_stream << fragment_stream.str();
			this->fragment_head_stream << "\n}\n";
			return this->fragment_head_stream.str();
		}

		std::string ExportVertex() {
            this->vertex_head_stream << "\n";
			this->vertex_head_stream << "$input ";
            unsigned long  i=0;
            for (const auto& uniformData : this->attributeParams) {
                this->vertex_head_stream << " " << uniformData.uniformName;
                i++;
                if(i<this->attributeParams.size()){
                    this->vertex_head_stream << ",";
                }
            }

            this->vertex_head_stream << " // in...\n";
            this->vertex_head_stream << "\n";
            this->vertex_head_stream << "$output ";
            
            i=0;
            for (const auto& uniformData : this->varyingParams) {
                this->vertex_head_stream << " " << uniformData.uniformName;
                i++;
                if(i<this->varyingParams.size()){
                    this->vertex_head_stream << ",";
                }
            }
            this->vertex_head_stream << " // out...\n";
            this->vertex_head_stream << "\n";
            this->vertex_head_stream << "#include \"../common/common.sh\"\n";
            this->vertex_head_stream << "#include \"../node/include/share_uniform.cginc\"\n";
            this->vertex_head_stream << "\n";
            
            for (const auto& uniformData : this->uniformVertexParams) {
                 this->vertex_head_stream << "uniform " << s_uniforType[uniformData.uniformType] << " " << uniformData.uniformName << ";\n";
            }

			this->vertex_head_stream << "void main(){\n";
			this->vertex_head_stream << vertex_stream.str();
			this->vertex_head_stream << "\n}\n";
			return this->vertex_head_stream.str();
		}
        
        
        std::vector<csc::UniformData> GetUniformData(const std::shared_ptr<cse::SharedState> shared_state){
            std::vector<csc::UniformData> uniforms;
            u_int16_t samplerIndex = 0;
            
            csc::UniformData rampSampler;
            auto rampTextureHandle =  shared_state->GetRampTextureHandler();//todo;
            rampSampler.uniformUnionData = UniformUnionData{TextureData{rampTextureHandle,9}};
            rampSampler.uniformId = shared_state->GetRampTextureUniformId();
            strcpy( rampSampler.uniformName , "ramp_curve");
            rampSampler.uniformType = csc::UniformType::Sampler;
            uniforms.push_back(rampSampler);
            
            for (const auto& uniformData : this->uniformFragParams) {
                csc::UniformData tmp;
                if(uniformData.uniformType == csc::UniformType::Sampler){
                    auto textureHandle =  shared_state->AddTextureHandler(uniformData.textFilePath);//todo;
                    tmp.uniformUnionData = UniformUnionData{TextureData{textureHandle,samplerIndex}};
                    samplerIndex++;
                }else{
                    tmp.uniformUnionData = UniformUnionData{uniformData.defaultValue};
                }
                tmp.uniformId = uniformData.uniformId;
                strcpy( tmp.uniformName , uniformData.uniformName);
                tmp.uniformType = uniformData.uniformType;
                uniforms.push_back(tmp);
            }
            return uniforms;//里面有些变量并不是在node中定义的,而是在codeh中定义的,但是需要声明 所以一并处理了
        }
        
        std::vector<uint64_t> get_connected_nodes(){
            std::vector<uint64_t> nodeIds;
            for (const auto& process : this->processedNodes) {
                nodeIds.push_back(process.first);
            }
            return nodeIds;
        }
	};

	std::shared_ptr<csg::CodeGenerateData> generate_graph_code( std::shared_ptr<Graph> graph,std::shared_ptr<cse::SharedState> sharedState);

    bool complie_graph( std::shared_ptr<Graph> graph,std::shared_ptr<cse::SharedState> sharedState,bool saveData2File);
}
