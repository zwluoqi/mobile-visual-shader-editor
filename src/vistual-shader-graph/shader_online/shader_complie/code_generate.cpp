/*
 * Copyright 2021-2021 Zhouwei. All rights reserved.
 * License: https://github.com/zwluoqi/mobile-visual-shader-editor#license-bsd-2-clause
 */

#include "code_generate.h"

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

#include <boost/tokenizer.hpp>

#include "../shader_core/config.h"
#include "../shader_core/rect.h"
#include "../shader_core/vector.h"

#include "curves.h"
#include "ext_base64.h"
#include "graph.h"
#include "node.h"
#include "node_enums.h"
#include "node_id.h"
#include "node_type.h"
#include "ramp.h"
#include "slot.h"
#include "slot_id.h"
#include "code_rampcolor_byte.h"


namespace csg {

        static std::string ResolveNode(std::shared_ptr<csg::CodeGenerateData> codeGenerateData,  const std::shared_ptr<const Node> node, int getOutIndex = 0);
        static std::string ResolveSlot(std::shared_ptr<csg::CodeGenerateData> codeGenerateData,  SlotId& slot, bool* connected);


        static const char* type_shadername_str[static_cast<int>(csg::SlotType::Count)+1] =
        {
            "float",//BOOL
            "Closure",//CLOSURE
            "vec4",//COLOR
            "float",//FLOAT
            "float",//INT
            "vec3",//VECTOR
            "Enumeration",//ENUM
            "float",//CURVE_RGB
            "float",//CURVE_VECTOR
            "float",//COLOR_RAMP
            "IMAGE",
            "[ERROR]"
        };

        static const csc::UniformType::Enum type_shadername_uniformType[static_cast<int>(csg::SlotType::Count)+1] =
        {
            csc::UniformType::Float,//BOOL,
            csc::UniformType::Vec4,//CLOSURE,
            csc::UniformType::Vec4,//COLOR,
            csc::UniformType::Float,//FLOAT,
            csc::UniformType::Float,//INT,
            csc::UniformType::Vec3,//VECTOR,
            csc::UniformType::Float,//ENUM,
            csc::UniformType::Float,//CURVE_RGB,
            csc::UniformType::Float,//CURVE_VECTOR,
            csc::UniformType::Float,//COLOR_RAMP,
            csc::UniformType::Sampler,//IMAGE,
            csc::UniformType::Vec4,//COUNT,
        };


        static const char* type_default_val_str[static_cast<int>(csg::SlotType::Count)+1] =
        {
            "0.0",//BOOL,
            "CLOSURE_DEFAULT",
            "vec4(1.0,1.0,1.0,1.0)",//COLOR,
            "0.0",//float
            "0.0",//int
            "vec3(1.0,1.0,1.0)",//vector
            "default_fun",//enum
            "0.0",//CURVE_RGB
            "0.0",//CURVE_VECTOR
            "0.0",//COLOR_RAMP
            "default_image",//image
            "error",
        };

        static std::string convert_type_shader_name(csg::SlotType s_type,csg::SlotType d_type,std::string s_slot_shader_name){
            std::stringstream ret;
            switch (s_type) {
                case SlotType::BOOL:
                case SlotType::FLOAT:
                case SlotType::INT:
                    switch (d_type) {
                        case SlotType::COLOR:
                        case SlotType::VECTOR:
                            ret<<type_shadername_str[static_cast<int>(d_type)];
                            ret<<"("<<s_slot_shader_name<<")";
                            return ret.str();
                            break;
                        case SlotType::FLOAT:
                        case SlotType::INT:
                        case SlotType::BOOL:
                            return s_slot_shader_name;
                            break;
                        default:
                            assert(false);
                            break;
                    }
                    break;
                case SlotType::COLOR:
                    switch (d_type) {
                        case SlotType::FLOAT:
                        case SlotType::INT:
                        case SlotType::BOOL:
                            ret<<s_slot_shader_name<<".x";
                            return ret.str();
                            break;
                        case SlotType::VECTOR:
                            ret<<s_slot_shader_name<<".xyz";
                            return ret.str();
                            break;
                        default:
                            assert(false);
                            break;
                    }
                    break;
                case SlotType::VECTOR:
                    switch (d_type) {
                        case SlotType::FLOAT:
                        case SlotType::INT:
                        case SlotType::BOOL:
                            ret<<s_slot_shader_name<<".x";
                            return ret.str();
                            break;
                        case SlotType::COLOR:
                            ret<<type_shadername_str[static_cast<int>(d_type)];
                            ret<<"("<<s_slot_shader_name<<",1.0)";
                            return ret.str();
                            break;
                        default:
                            break;
                    }
                    break;
                default:
                    assert(false);
                    break;
            }
            return s_slot_shader_name;
        }

        static const std::string val_glsl_str(csc::Float3 val)
        {
            std::stringstream ret;
            ret<<"vec3("<<val.x<<","<<val.y<<","<<val.z<<");";
            return ret.str();
        }

        static const std::string val_glsl_str(csc::Float4 val)
        {
            std::stringstream ret;
            ret<<"vec4("<<val.x<<","<<val.y<<","<<val.z<<","<<val.w<<");";
            return ret.str();
        }


        static UniformCodeData ConvertToUniformData(const std::string& _uniformName,const std::string& _defaultVal){
            UniformCodeData uniformData;
            strncpy(uniformData.uniformName, _uniformName.c_str(),_uniformName.length());
            uniformData.uniformName[_uniformName.length()] = 0;
            strncpy(uniformData.defaultVal, _defaultVal.c_str(),_defaultVal.length());
            uniformData.defaultVal[_defaultVal.length()] = 0;
            return uniformData;
        }

        static void AddMVPUniform(std::shared_ptr<csg::CodeGenerateData> codeGenerateData) {
            //default val
        }


        static void AddLightUniform(std::shared_ptr<csg::CodeGenerateData> codeGenerateData) {
            codeGenerateData->AddUniformParams(UniformCodeData{ ShaderDataType::varying, "vWorldToLight",csc::UniformType::Mat4 });
            codeGenerateData->AddUniformParams(UniformCodeData{ ShaderDataType::varying, "vPositionFromLight",csc::UniformType::Vec4 });
            codeGenerateData->vertex_stream << " vWorldToLight = uLightVP;\n";
            codeGenerateData->vertex_stream << " vPositionFromLight = uLightMVP * vec4(a_position, 1.0);\n";
            
            codeGenerateData->fragment_stream << "vec3 lightDir = normalize(uLightDir.xyz);\n";
            codeGenerateData->fragment_stream << "vec3 lightRadiance = uLightRadiance.xyz;\n";

        }

        static void AddPosAttribute(std::shared_ptr<csg::CodeGenerateData> codeGenerateData) {
            codeGenerateData->AddUniformParams(UniformCodeData{ ShaderDataType::attribute,"a_position",csc::UniformType::Vec3,ShaderAttributeType::Position });
            codeGenerateData->AddUniformParams(UniformCodeData{ ShaderDataType::attribute,"a_texcoord0",csc::UniformType::Vec2});

            codeGenerateData->AddUniformParams(UniformCodeData{ ShaderDataType::varying,"GPU_WORLD_POSITION",csc::UniformType::Vec3,ShaderAttributeType::Position});//世界空间位置
            codeGenerateData->AddUniformParams(UniformCodeData{ ShaderDataType::varying,"VERTEX_CD_MTFACE",csc::UniformType::Vec3 });//UV坐标
            codeGenerateData->AddUniformParams(UniformCodeData{ ShaderDataType::varying,"VERTEX_CD_ORCO",csc::UniformType::Vec3 });//顶点坐标
            //codeGenerateData->AddUniformParams(UniformCodeData{ ShaderDataType::varying,"VERTEX_CD_MCOL",csc::UniformType::Vec4 });//顶点颜色

            
            
            codeGenerateData->vertex_stream << " GPU_WORLD_POSITION = mul(u_model[0] , vec4(a_position, 1.0)).xyz;\n";
            codeGenerateData->vertex_stream << " VERTEX_CD_MTFACE = vec3(a_texcoord0,0.0);\n";
            codeGenerateData->vertex_stream << " VERTEX_CD_ORCO = a_position;\n";

            codeGenerateData->vertex_stream << " gl_Position = mul(u_modelViewProj ,vec4(a_position, 1.0));\n";


            codeGenerateData->fragment_stream << "vec3 viewDir = mul(u_view , vec4(GPU_WORLD_POSITION,1.0));\n";
            codeGenerateData->fragment_stream << "vec4 colorRamp = texture(ramp_curve,VERTEX_CD_MTFACE);\n";
            codeGenerateData->fragment_stream << "vec3 GPU_VIEW_POSITION = mul(u_view , vec4(GPU_WORLD_POSITION, 1.0)).xyz;\n";
            codeGenerateData->fragment_stream << "vec2 GPU_BARYCENTRIC_TEXCO=vec2(0.0,0.0);\n";
            codeGenerateData->fragment_stream << "vec3 GPU_BARYCENTRIC_DIST=vec3(0.0,0.0,0.0);\n";
            codeGenerateData->fragment_stream << "vec4 GPU_CAMERA_TEXCO_FACTORS = vec4(1.0,1.0,0.0,0.0);\n";
            
        }

        static void AddNormalAttribute(std::shared_ptr<csg::CodeGenerateData> codeGenerateData) {
            codeGenerateData->AddUniformParams(UniformCodeData{ ShaderDataType::attribute,"a_normal",csc::UniformType::Vec3 ,ShaderAttributeType::Normal});
            codeGenerateData->AddUniformParams(UniformCodeData{ ShaderDataType::attribute,"a_tangent",csc::UniformType::Vec4 ,ShaderAttributeType::Tangent});

            codeGenerateData->AddUniformParams(UniformCodeData{ ShaderDataType::varying,"GPU_WORLD_NORMAL",csc::UniformType::Vec3         });//世界空间法线,TODO也许有问题，发现插值导致在顶点着色数据不对
            codeGenerateData->AddUniformParams(UniformCodeData{ ShaderDataType::varying,"VERTEX_CD_TANGENT",csc::UniformType::Vec4 });//顶点切线

            codeGenerateData->vertex_stream << " GPU_WORLD_NORMAL = mul(transpose((u_model[1])) , vec4(a_normal, 0.0)).xyz;\n";
            codeGenerateData->vertex_stream << " VERTEX_CD_TANGENT = a_tangent;\n";
        }


        static std::string code_slot_value(std::shared_ptr<csg::CodeGenerateData> codeGenerateData,
                                           SlotId& slotId,
                                           const csg::SlotValue& slot_value,
                                           Float4& outFloat4Value,
                                           std::string& outSamplerFilePath)
        {
            switch (slot_value.type()) {
            case csg::SlotType::BOOL:
                if (slot_value.as<csg::BoolSlotValue>().has_value()) {
                    const csg::BoolSlotValue bool_slot_value{ slot_value.as<csg::BoolSlotValue>().value() };
                    if (bool_slot_value.get()) {
                        outFloat4Value.x = 1;
                        return "1";
                    }
                    else {
                        outFloat4Value.x = 0;
                        return "0";
                    }
                }
                break;
            case csg::SlotType::COLOR:
                if (slot_value.as<csg::ColorSlotValue>().has_value()) {
                    const csg::ColorSlotValue color_slot_value{ slot_value.as<csg::ColorSlotValue>().value() };
                    const csc::Float3 color{ color_slot_value.get() };
                    outFloat4Value.x = color.x;
                    outFloat4Value.y = color.y;
                    outFloat4Value.z = color.z;
                    std::stringstream sstream;
                    sstream << std::fixed << std::setprecision(SERIALIZED_GRAPH_PRECISION) << type_shadername_str[static_cast<int>(slot_value.type())] << "(" << color.x << ',' << color.y << ',' << color.z<<",1.0f" << ")";
                    return sstream.str();
                }
                break;
            case csg::SlotType::ENUM:
                if (slot_value.as<csg::EnumSlotValue>().has_value()) {
                    const csg::EnumSlotValue enum_slot_value{ slot_value.as<csg::EnumSlotValue>().value() };
                    outFloat4Value.x = enum_slot_value.get()*1.0;
                    return std::string{ enum_slot_value.internal_name() };
                }
                break;
            case csg::SlotType::FLOAT:
                if (slot_value.as<csg::FloatSlotValue>().has_value()) {
                    const csg::FloatSlotValue float_slot_value{ slot_value.as<csg::FloatSlotValue>().value() };
                    outFloat4Value.x = float_slot_value.get();
                    std::stringstream sstream;
                    sstream << std::fixed << std::setprecision(SERIALIZED_GRAPH_PRECISION) << float_slot_value.get();
                    return sstream.str();
                }
                break;
            case csg::SlotType::INT:
                if (slot_value.as<csg::IntSlotValue>().has_value()) {
                    const csg::IntSlotValue int_slot_value{ slot_value.as<csg::IntSlotValue>().value() };
                    outFloat4Value.x = int_slot_value.get();
                    std::stringstream sstream;
                    sstream << int_slot_value.get();
                    return sstream.str();
                }
                break;
            case csg::SlotType::VECTOR:
                if (slot_value.as<csg::VectorSlotValue>().has_value()) {
                    const csg::VectorSlotValue vec_slot_value{ slot_value.as<csg::VectorSlotValue>().value() };
                    const csc::Float3 color{ vec_slot_value.get() };
                    outFloat4Value.x = color.x;
                    outFloat4Value.y = color.y;
                    outFloat4Value.z = color.z;
                    std::stringstream sstream;
                    sstream << std::fixed << std::setprecision(SERIALIZED_GRAPH_PRECISION) << type_shadername_str[static_cast<int>(slot_value.type())] << "(" << color.x << ',' << color.y << ',' << color.z<<")";
                    return sstream.str();
                }
                break;

            case csg::SlotType::CURVE_RGB:
            	if (slot_value.as<csg::RGBCurveSlotValue>().has_value()) {
//            		constexpr char CURVE_SEPARATOR{ '/' };
            		const csg::RGBCurveSlotValue rgb_slot_value{ slot_value.as<csg::RGBCurveSlotValue>().value() };
            		std::stringstream sstream;
                    
                    
                    auto slotName = codeGenerateData->graph->GetSlotName(slotId);
                    csc::Float4 range;
                    csc::Float4 ext_x;
                    csc::Float4 ext_y;
                    csc::Float4 ext_z;
                    csc::Float4 ext_w;
                    auto samplerY = codeGenerateData->current_layer;
                    codeGenerateData->current_layer = SetRGBCurveSlotValue(codeGenerateData->shareState,&rgb_slot_value,codeGenerateData->current_layer,&range,&ext_x,&ext_y,&ext_z,&ext_w);
                    codeGenerateData->shareState->setSlotLayer(slotId.slot_id(), samplerY);
                    //TODO 理论上应该设置为uuniform
                    codeGenerateData->fragment_stream<<"vec4 "<<slotName<<"_range="<<val_glsl_str(range)<<";\n";
                    codeGenerateData->fragment_stream<<"vec4 "<<slotName<<"_ext_x="<<val_glsl_str(ext_x)<<";\n";
                    codeGenerateData->fragment_stream<<"vec4 "<<slotName<<"_ext_y="<<val_glsl_str(ext_y)<<";\n";
                    codeGenerateData->fragment_stream<<"vec4 "<<slotName<<"_ext_z="<<val_glsl_str(ext_z)<<";\n";
                    codeGenerateData->fragment_stream<<"vec4 "<<slotName<<"_ext_w="<<val_glsl_str(ext_w)<<";\n";
                    sstream << samplerY;
                    
                    return sstream.str();
            	}
            	break;
            case csg::SlotType::CURVE_VECTOR:
            	if (slot_value.as<csg::VectorCurveSlotValue>().has_value()) {
            	//	constexpr char CURVE_SEPARATOR{ '/' };
            		const csg::VectorCurveSlotValue curve_slot_value{ slot_value.as<csg::VectorCurveSlotValue>().value() };
            		std::stringstream sstream;
                    
                    auto slotName = codeGenerateData->graph->GetSlotName(slotId);
                    csc::Float3 range;
                    csc::Float4 ext_x;
                    csc::Float4 ext_y;
                    csc::Float4 ext_z;
                    auto samplerY = codeGenerateData->current_layer;
                    codeGenerateData->current_layer = SetVectorCurveSlotValue(codeGenerateData->shareState,&curve_slot_value,codeGenerateData->current_layer,&range,&ext_x,&ext_y,&ext_z);
                    codeGenerateData->shareState->setSlotLayer(slotId.slot_id(), samplerY);
                    codeGenerateData->fragment_stream<<"vec3 "<<slotName<<"_range="<<val_glsl_str(range)<<";\n";
                    codeGenerateData->fragment_stream<<"vec4 "<<slotName<<"_ext_x="<<val_glsl_str(ext_x)<<";\n";
                    codeGenerateData->fragment_stream<<"vec4 "<<slotName<<"_ext_y="<<val_glsl_str(ext_y)<<";\n";
                    codeGenerateData->fragment_stream<<"vec4 "<<slotName<<"_ext_z="<<val_glsl_str(ext_z)<<";\n";
                    sstream << samplerY;
                    return sstream.str();
            	}
            	break;
            case csg::SlotType::COLOR_RAMP:
                if (slot_value.as<csg::ColorRampSlotValue>().has_value()) {
                    csg::ColorRampSlotValue ramp_slot_value{ slot_value.as<csg::ColorRampSlotValue>().value() };
                    
                    auto samplerY = codeGenerateData->current_layer;
                    codeGenerateData->current_layer = SetColorRampSlotValue(codeGenerateData->shareState,&ramp_slot_value,codeGenerateData->current_layer);
                    codeGenerateData->shareState->setSlotLayer(slotId.slot_id(), samplerY);
                    
                    std::stringstream sstream;
                    sstream << samplerY;
                    return sstream.str();
                }
                break;
                    
            case csg::SlotType::IMAGE:
                if (slot_value.as<csg::ImageSlotValue>().has_value()) {
                   const csg::ImageSlotValue int_slot_value{ slot_value.as<csg::ImageSlotValue>().value() };
                   outSamplerFilePath = int_slot_value.get();
                   std::stringstream sstream;
                   sstream << int_slot_value.get();
                   return sstream.str();
               }
                break;
            default:
                assert(false);
                break;
            }

            return "ERROR";
        }

        static const char* GetSlotDataType(std::shared_ptr<Graph> graph,  SlotId& slot) {
            const auto dest_node{ graph->get(slot.node_id()) };
            const auto opt_slot{ dest_node->slot(slot.index()) };
            return type_shadername_str[static_cast<int>(opt_slot->type())];
        }

        static csc::UniformType::Enum GetSlotUniformType(std::shared_ptr<Graph> graph,  SlotId& slot) {
            const auto dest_node{ graph->get(slot.node_id()) };
            const auto opt_slot{ dest_node->slot(slot.index()) };
            return type_shadername_uniformType[static_cast<int>(opt_slot->type())];
        }

        static const char* GetSlotDataTypeDefaultValue(std::shared_ptr<Graph> graph,  SlotId& slot) {
            const auto dest_node{ graph->get(slot.node_id()) };
            const auto opt_slot{ dest_node->slot(slot.index()) };
            return type_default_val_str[static_cast<int>(opt_slot->type())];
        }

        static std::string ResolveSlot(std::shared_ptr<csg::CodeGenerateData> codeGenerateData,SlotId& slotId,bool* connected) {
            auto graph=codeGenerateData->graph;
            auto connect = graph->get_connection(slotId);
            auto node = graph->get(slotId.node_id());
            auto slot = node->slot(slotId.index());
            
            *connected = connect.has_value();
            if (connect.has_value()) {
                auto source_slot_id = connect->source();
                const auto source_node{ graph->get(source_slot_id.node_id()) };
                if(source_node != nullptr){
                    auto source_connect_slot_name = ResolveNode(codeGenerateData, source_node, (int)connect->source().index());
                    auto source_slot = source_node->slot(source_slot_id.index());
                    if(source_slot->type() == slot->type()){
                        return source_connect_slot_name;
                    }else{
                        return convert_type_shader_name(source_slot->type(),slot->type(),source_connect_slot_name);
                    }
                }else{
                    printf("not has source node by connect");
                }
            }
            
            //没有连线的,
            //1.作为uniform为外界当做mateial的参数传送
            //2.使用内部申明的vvarying值
            auto gpuName = slot->getGPUVaryName();
            if(gpuName != nullptr){
                return gpuName;
            }else{
                auto slotValue0 = graph->get_slot_value(slotId);
                std::string outValueStr;
                Float4 outValue;
                
                std::string samplerTexFilePath;

                if (slotValue0.has_value()) {
                    outValueStr = code_slot_value(codeGenerateData,
                                                      slotId,
                                                      slotValue0.value(),
                                                      outValue,
                                                      samplerTexFilePath);
                }
                else {
                    outValueStr = GetSlotDataTypeDefaultValue(graph, slotId);
                }
                std::string outName = graph->GetSlotName(slotId);
               
                if(slot->type() == SlotType::CLOSURE)
                {
                    auto outTypeStr = GetSlotDataType(graph, slotId);
                    codeGenerateData->fragment_stream<<outTypeStr<<" "<<outName<<"="<<outValueStr<<";\n";
                }
                else
                {
                    auto uniformType = GetSlotUniformType(graph, slotId);
                    UniformCodeData uniformData = ConvertToUniformData(outName,outValueStr);
                    uniformData.dataType = ShaderDataType::uniformFrag;
                    uniformData.uniformType = uniformType;
                    uniformData.defaultValue = outValue;
                    uniformData.uniformId = slotId.slot_id();
                    strcpy(uniformData.textFilePath, samplerTexFilePath.c_str());
                    
                    codeGenerateData->AddUniformParams(uniformData);
                }
                
                return outName;
            }
        }


        static void ProcessNode(std::shared_ptr<csg::CodeGenerateData> codeGenerateData,
                                const std::shared_ptr<const Node> node,
                                const char* fun_name,
                                std::vector<std::string>& p_outVals,
                                int funNameMask = 0,
                                std::vector<std::string>* ext_inputVals = nullptr) {
            auto graph=codeGenerateData->graph;
            auto slotCount = node->slots().size();
            auto funParamsCount = slotCount;

            bool connected;

            std::vector<std::string> p_inputVals;

            std::vector<const char *> inputVals;
            std::vector<const char *> outParam;
            
            std::stringstream newFunPreName;
            for (size_t i = 0; i < slotCount; i++)
            {
                SlotId slotIdi = SlotId(node->id(), i);

                //this is fun name
                if ((funNameMask & (2<<i)) > 0 ) {
                    auto slotValue0 = graph->get_slot_value(slotIdi);
                    Float4 value;
                    std::string samplerTexFilePath;
                    std::string funMaskPrefix = code_slot_value(codeGenerateData, slotIdi, slotValue0.value(),value,samplerTexFilePath);
                    if(funParamsCount != slotCount){
                        newFunPreName <<"_"<< funMaskPrefix;
                    }else{
                        newFunPreName << funMaskPrefix;
                    }
                    funParamsCount--;
                    continue;
                }

                //this is out param
                if (node->slot_dir(i) == SlotDirection::OUTPUT) {
                    std::string outType = GetSlotDataType(graph, slotIdi);
                    std::string outValue = graph->GetSlotName(slotIdi);
                    codeGenerateData->fragment_stream << " " << outType << " " << outValue <<";\n";
                    p_outVals.push_back(outValue);
                }
                //this is in param
                else {
                    std::string inputValue = ResolveSlot(codeGenerateData, slotIdi, &connected);
                    p_inputVals.push_back(inputValue);
                }
            }
            newFunPreName << fun_name;
            std::string newFunName = newFunPreName.str();
            

            std::stringstream error;
            for(int i=0;i<p_inputVals.size();i++){
                inputVals.push_back(&p_inputVals[i][0]);
            }
            if(ext_inputVals != nullptr){
                for(int i=0;i<ext_inputVals->size();i++){
                    inputVals.push_back(&ext_inputVals->at(i)[0]);
                }
                funParamsCount += ext_inputVals->size();
            }

            for(int j=0;j<p_outVals.size();j++){
                outParam.push_back(&p_outVals[j][0]);
            }
            
            char buff[1024];
            const char* new_fun_name_ptr = newFunName.c_str();
            if (funParamsCount == 2) {
                if (outParam.size() == 1) {
                    snprintf(buff, sizeof(buff), new_fun_name_ptr, inputVals[0], outParam[0]);
                }
                else {
                    assert(false);
                }
            }
            else if (funParamsCount == 3) {
                if (outParam.size() == 1) {
                    snprintf(buff, sizeof(buff), new_fun_name_ptr, inputVals[0], inputVals[1],outParam[0]);
                }
                else if (outParam.size() == 2) {
                    snprintf(buff, sizeof(buff), new_fun_name_ptr, inputVals[0],  outParam[0], outParam[1]);
                }
                else {
                    assert(false);
                }
            }
            else if (funParamsCount == 4) {
                if (outParam.size() == 1) {
                    snprintf(buff, sizeof(buff), new_fun_name_ptr, inputVals[0], inputVals[1], inputVals[2], outParam[0]);
                }
                else if (outParam.size() == 2) {
                    snprintf(buff, sizeof(buff), new_fun_name_ptr, inputVals[0], inputVals[1], outParam[0], outParam[1]);
                }
                else if (outParam.size() == 3) {
                    snprintf(buff, sizeof(buff), new_fun_name_ptr, inputVals[0],  outParam[0], outParam[1], outParam[2]);
                }
                else {
                    assert(false);
                }
            }
            else if (funParamsCount == 5) {
                if (outParam.size() == 1) {
                    snprintf(buff, sizeof(buff), new_fun_name_ptr, inputVals[0], inputVals[1], inputVals[2], inputVals[3], outParam[0]);
                }
                else if (outParam.size() == 2) {
                    snprintf(buff, sizeof(buff), new_fun_name_ptr, inputVals[0], inputVals[1], inputVals[2], outParam[0], outParam[1]);
                }
                else if (outParam.size() == 3) {
                    snprintf(buff, sizeof(buff), new_fun_name_ptr, inputVals[0], inputVals[1], outParam[0], outParam[1], outParam[2]);
                }
                else {
                    assert(false);
                }
            }
            else if (funParamsCount == 6) {
                if (outParam.size() == 1) {

                    snprintf(buff, sizeof(buff), new_fun_name_ptr, inputVals[0], inputVals[1], inputVals[2], inputVals[3], inputVals[4], outParam[0]);
                }
                else if (outParam.size() == 2) {
                    snprintf(buff, sizeof(buff), new_fun_name_ptr, inputVals[0], inputVals[1], inputVals[2], inputVals[3], outParam[0], outParam[1]);
                }
                else if (outParam.size() == 3) {
                    snprintf(buff, sizeof(buff), new_fun_name_ptr, inputVals[0], inputVals[1], inputVals[2], outParam[0], outParam[1], outParam[2]);
                }
                else {
                    assert(false);
                }
            }
            else if (funParamsCount == 7) {
                if (outParam.size() == 1) {

                    snprintf(buff, sizeof(buff), new_fun_name_ptr, inputVals[0], inputVals[1], inputVals[2], inputVals[3], inputVals[4], inputVals[5], outParam[0]);
                }
                else if (outParam.size() == 2) {
                    snprintf(buff, sizeof(buff), new_fun_name_ptr, inputVals[0], inputVals[1], inputVals[2], inputVals[3], inputVals[4], outParam[0], outParam[1]);
                }
                else if (outParam.size() == 3) {
                    snprintf(buff, sizeof(buff), new_fun_name_ptr, inputVals[0], inputVals[1], inputVals[2], inputVals[3], outParam[0], outParam[1], outParam[2]);
                }
                else {
                    assert(false);
                }
            }
            else if (funParamsCount == 8) {
                if (outParam.size() == 1) {

                    snprintf(buff, sizeof(buff), new_fun_name_ptr, inputVals[0], inputVals[1], inputVals[2], inputVals[3], inputVals[4], inputVals[5], inputVals[6], outParam[0]);
                }
                else if (outParam.size() == 2) {
                    snprintf(buff, sizeof(buff), new_fun_name_ptr, inputVals[0], inputVals[1], inputVals[2], inputVals[3], inputVals[4], inputVals[5], outParam[0], outParam[1]);
                }
                else if (outParam.size() == 3) {
                    snprintf(buff, sizeof(buff), new_fun_name_ptr, inputVals[0], inputVals[1], inputVals[2], inputVals[3], inputVals[4], outParam[0], outParam[1], outParam[2]);
                }
                else if (outParam.size() == 4) {
                    snprintf(buff, sizeof(buff), new_fun_name_ptr, inputVals[0], inputVals[1], inputVals[2], inputVals[3],  outParam[0], outParam[1], outParam[2],outParam[3]);
                }
                else if (outParam.size() == 5) {
                    snprintf(buff, sizeof(buff), new_fun_name_ptr, inputVals[0], inputVals[1], inputVals[2], outParam[0], outParam[1], outParam[2],outParam[3],outParam[4]);
                }
                else {
                   assert(false);
                }
            }
            else if (funParamsCount == 9) {
                if (outParam.size() == 1) {

                    snprintf(buff, sizeof(buff), new_fun_name_ptr, inputVals[0], inputVals[1], inputVals[2], inputVals[3], inputVals[4], inputVals[5], inputVals[6], inputVals[7], outParam[0]);
                }
                else if (outParam.size() == 2) {
                    snprintf(buff, sizeof(buff), new_fun_name_ptr, inputVals[0], inputVals[1], inputVals[2], inputVals[3], inputVals[4], inputVals[5], inputVals[6], outParam[0], outParam[1]);
                }
                else if (outParam.size() == 3) {
                    snprintf(buff, sizeof(buff), new_fun_name_ptr, inputVals[0], inputVals[1], inputVals[2], inputVals[3], inputVals[4], inputVals[5], outParam[0], outParam[1], outParam[2]);
                }
                else {
                    assert(false);
                }
            }
            else if (funParamsCount == 10) {
                if (outParam.size() == 1) {

                    snprintf(buff, sizeof(buff), new_fun_name_ptr, inputVals[0], inputVals[1], inputVals[2], inputVals[3], inputVals[4], inputVals[5], inputVals[6], inputVals[7], inputVals[8], outParam[0]);
                }
                else if (outParam.size() == 2) {
                    snprintf(buff, sizeof(buff), new_fun_name_ptr, inputVals[0], inputVals[1], inputVals[2], inputVals[3], inputVals[4], inputVals[5], inputVals[6], inputVals[7], outParam[0], outParam[1]);
                }
                else if (outParam.size() == 3) {
                    snprintf(buff, sizeof(buff), new_fun_name_ptr, inputVals[0], inputVals[1], inputVals[2], inputVals[3], inputVals[4], inputVals[5], inputVals[6], outParam[0], outParam[1], outParam[2]);
                }
                else if (outParam.size() == 4) {
                    snprintf(buff, sizeof(buff), new_fun_name_ptr, inputVals[0], inputVals[1], inputVals[2], inputVals[3], inputVals[4], inputVals[5], outParam[0], outParam[1], outParam[2],outParam[3]);
                }
                else {
                   assert(false);
                }
            }
            else if (funParamsCount == 11) {
                if (outParam.size() == 1) {

                    snprintf(buff, sizeof(buff), new_fun_name_ptr, inputVals[0], inputVals[1], inputVals[2], inputVals[3], inputVals[4], inputVals[5], inputVals[6], inputVals[7], inputVals[8], inputVals[9], outParam[0]);
                }
                else if (outParam.size() == 2) {
                    snprintf(buff, sizeof(buff), new_fun_name_ptr, inputVals[0], inputVals[1], inputVals[2], inputVals[3], inputVals[4], inputVals[5], inputVals[6], inputVals[7], inputVals[8], outParam[0], outParam[1]);
                }
                else if (outParam.size() == 3) {
                    snprintf(buff, sizeof(buff), new_fun_name_ptr, inputVals[0], inputVals[1], inputVals[2], inputVals[3], inputVals[4], inputVals[5], inputVals[6], inputVals[7], outParam[0], outParam[1], outParam[2]);
                }
                else if (outParam.size() == 4) {
                    snprintf(buff, sizeof(buff), new_fun_name_ptr, inputVals[0], inputVals[1], inputVals[2], inputVals[3], inputVals[4], inputVals[5], inputVals[6], outParam[0], outParam[1], outParam[2],outParam[3]);
                }
                else {
                    assert(false);
                }
            }
            else if (funParamsCount == 12) {
                if (outParam.size() == 1) {
                    snprintf(buff, sizeof(buff), new_fun_name_ptr, inputVals[0], inputVals[1], inputVals[2], inputVals[3], inputVals[4], inputVals[5], inputVals[6], inputVals[7], inputVals[8], inputVals[9], inputVals[10], outParam[0]);
                }
                else if (outParam.size() == 2) {
                    snprintf(buff, sizeof(buff), new_fun_name_ptr, inputVals[0], inputVals[1], inputVals[2], inputVals[3], inputVals[4], inputVals[5], inputVals[6], inputVals[7], inputVals[8], inputVals[9], outParam[0], outParam[1]);
                }
                else if (outParam.size() == 3) {
                    snprintf(buff, sizeof(buff), new_fun_name_ptr, inputVals[0], inputVals[1], inputVals[2], inputVals[3], inputVals[4], inputVals[5], inputVals[6], inputVals[7], inputVals[8], outParam[0], outParam[1], outParam[2]);
                }
                else if (outParam.size() == 4) {
                    snprintf(buff, sizeof(buff), new_fun_name_ptr, inputVals[0], inputVals[1], inputVals[2], inputVals[3], inputVals[4], inputVals[5], inputVals[6], inputVals[7], outParam[0], outParam[1], outParam[2], outParam[3]);
                }
                else if (outParam.size() == 5) {
                    snprintf(buff, sizeof(buff), new_fun_name_ptr, inputVals[0], inputVals[1], inputVals[2], inputVals[3], inputVals[4], inputVals[5], inputVals[6], outParam[0], outParam[1], outParam[2], outParam[3],outParam[4]);
                }
                else if (outParam.size() == 7) {
                   snprintf(buff, sizeof(buff), new_fun_name_ptr, inputVals[0], inputVals[1], inputVals[2], inputVals[3], inputVals[4],  outParam[0], outParam[1], outParam[2], outParam[3],outParam[4],outParam[5],outParam[6]);
               }
                else {
                    assert(false);
                }
            }
            else if (funParamsCount == 13) {
                if (outParam.size() == 7) {
                    snprintf(buff, sizeof(buff), new_fun_name_ptr, inputVals[0], inputVals[1], inputVals[2], inputVals[3], inputVals[4],inputVals[5],outParam[0], outParam[1], outParam[2], outParam[3],outParam[4],outParam[5],outParam[6]);
                }
                 else {
                     assert(false);
                 }
            }
            else if (funParamsCount == 14) {
                if (outParam.size() == 1) {
                    snprintf(buff, sizeof(buff), new_fun_name_ptr, inputVals[0], inputVals[1], inputVals[2], inputVals[3], inputVals[4], inputVals[5], inputVals[6], inputVals[7], inputVals[8], inputVals[9], inputVals[10], inputVals[11], inputVals[12], outParam[0]);
                }
                else if (outParam.size() == 2) {
                    snprintf(buff, sizeof(buff), new_fun_name_ptr, inputVals[0], inputVals[1], inputVals[2], inputVals[3], inputVals[4], inputVals[5], inputVals[6], inputVals[7], inputVals[8], inputVals[9],inputVals[10], inputVals[11],  outParam[0], outParam[1]);
                }
                else if (outParam.size() == 3) {
                    snprintf(buff, sizeof(buff), new_fun_name_ptr, inputVals[0], inputVals[1], inputVals[2], inputVals[3], inputVals[4], inputVals[5], inputVals[6], inputVals[7], inputVals[8], inputVals[9],inputVals[10],outParam[0], outParam[1], outParam[2]);
                }
                else if (outParam.size() == 4) {
                    snprintf(buff, sizeof(buff), new_fun_name_ptr, inputVals[0], inputVals[1], inputVals[2], inputVals[3], inputVals[4], inputVals[5], inputVals[6], inputVals[7], inputVals[8], inputVals[9],outParam[0], outParam[1], outParam[2], outParam[3]);
                }
                else if (outParam.size() == 5) {
                    snprintf(buff, sizeof(buff), new_fun_name_ptr, inputVals[0], inputVals[1], inputVals[2], inputVals[3], inputVals[4], inputVals[5], inputVals[6], inputVals[7], inputVals[8],outParam[0], outParam[1], outParam[2], outParam[3],outParam[4]);
                }
                else if (outParam.size() == 9) {
                    snprintf(buff, sizeof(buff), new_fun_name_ptr, inputVals[0], inputVals[1], inputVals[2], inputVals[3], inputVals[4], outParam[0], outParam[1], outParam[2], outParam[3],outParam[4],outParam[5], outParam[6], outParam[7], outParam[8]);
                }
                else {
                    assert(false);
                }
            }
            else if (funParamsCount == 16) {
                if (outParam.size() == 1) {

                    snprintf(buff, sizeof(buff), new_fun_name_ptr, inputVals[0], inputVals[1], inputVals[2], inputVals[3], inputVals[4], inputVals[5], inputVals[6], inputVals[7], inputVals[8], inputVals[9], inputVals[10],inputVals[11],inputVals[12],inputVals[13],inputVals[14], outParam[0]);
                }
                else if (outParam.size() == 2) {
                    snprintf(buff, sizeof(buff), new_fun_name_ptr, inputVals[0], inputVals[1], inputVals[2], inputVals[3], inputVals[4], inputVals[5], inputVals[6], inputVals[7], inputVals[8], inputVals[9],inputVals[10],inputVals[11],inputVals[12],inputVals[13], outParam[0], outParam[1]);
                }
                else if (outParam.size() == 3) {
                    snprintf(buff, sizeof(buff), new_fun_name_ptr, inputVals[0], inputVals[1], inputVals[2], inputVals[3], inputVals[4], inputVals[5], inputVals[6], inputVals[7], inputVals[8],inputVals[10],inputVals[11],inputVals[12], outParam[0], outParam[1], outParam[2]);
                }
                else {
                    assert(false);
                }
            }
            else {
                assert(false);
            }
            codeGenerateData->fragment_stream << " "<< buff << ";\n";
            return;
        }

        static std::string ResolveNode(std::shared_ptr<csg::CodeGenerateData> codeGenerateData, const std::shared_ptr<const Node> node,int getOutIndex) {
            auto graph=codeGenerateData->graph;
            
            //已经处理过的节点
            if(codeGenerateData->hasProcessedNode(node->id())){
                SlotId slotIdIndex = SlotId(node->id(), getOutIndex);
                std::string outValue = graph->GetSlotName(slotIdIndex);
                return outValue;
            }
            
            //标记处理该节点
            codeGenerateData->processedNode(node->id());

            switch (node->type()) {
                //////
                // Output
                //////
            case NodeType::MATERIAL_OUTPUT:
                {
                    bool connected;
                    SlotId slotId0 = SlotId(node->id(), 0);
                    std::string surface = ResolveSlot(codeGenerateData, slotId0,&connected);
                    codeGenerateData->fragment_stream << " gl_FragColor = " << surface <<".radiance;" << "\n";
                    return "";
                }
                break;
            //	//////
            //	// Color
            //	//////
            case NodeType::BRIGHTNESS_CONTRAST:
                {
                    std::vector<std::string> outParam;
                    ProcessNode(codeGenerateData, node, "node_brightness_contrast(%s,%s,%s,%s)", outParam);
                    return outParam[0];
                }
                break;
            case NodeType::GAMMA:
            {
                std::vector<std::string> outParam;
                ProcessNode(codeGenerateData, node, "node_gamma(%s,%s,%s)",  outParam);
                return outParam[0];
            }
                break;
            case NodeType::HSV:
            {
                std::vector<std::string> outParam;
                ProcessNode(codeGenerateData, node, "node_hsv(%s,%s,%s,%s,%s,%s)",  outParam);
                return outParam[getOutIndex];
            }
                break;
            case NodeType::INVERT:
            {
                std::vector<std::string> outParam;
                ProcessNode(codeGenerateData, node, "node_invert(%s,%s,%s)",  outParam);
                return outParam[getOutIndex];
            }
                break;
            case NodeType::LIGHT_FALLOFF:
            {
                std::vector<std::string> outParam;
                ProcessNode(codeGenerateData, node, "node_light_falloff(%s,%s,%s,%s,%s)",  outParam);
                return outParam[getOutIndex];
            }
                break;
            case NodeType::MIX_RGB:
            {
                std::vector<std::string> outParam;
                ProcessNode(codeGenerateData, node, "(%s,%s,%s,%s,%s)",  outParam,2<<1);
                return outParam[getOutIndex];
            }
                break;
            case NodeType::RGB_CURVES:
            {
            	std::vector<std::string> outParam;
                
                SlotId slotId0 = SlotId(node->id(), 1);
                std::string outValue = graph->GetSlotName(slotId0);
                
                std::vector<std::string> extInput;
                extInput.push_back(outValue+"_range");
                extInput.push_back(outValue+"_ext_x");
                extInput.push_back(outValue+"_ext_y");
                extInput.push_back(outValue+"_ext_z");
                extInput.push_back(outValue+"_ext_w");
                
            	ProcessNode(codeGenerateData, node, "curves_rgb(%s,%s,%s,%s,%s,%s,%s,%s,%s)", outParam,0,&extInput);
            	return outParam[getOutIndex];
            }
            	break;
            //	//////
            //	// Converter
            //	//////
            case NodeType::BLACKBODY:
            {
                std::vector<std::string> outParam;
                ProcessNode(codeGenerateData,  node, "node_blackbody(%s,%s)",  outParam);
                return outParam[getOutIndex];
            }
                break;
            case NodeType::CLAMP:
            {
                std::vector<std::string> outParam;
                ProcessNode(codeGenerateData,  node, "(%s,%s,%s,%s)",  outParam,2<<1);
                return outParam[getOutIndex];
            }
                break;
            case NodeType::COLOR_RAMP:
            {
                SlotId slotId0 = SlotId(node->id(), 0);
                SlotId slotId1 = SlotId(node->id(), 1);
                SlotId slotId2 = SlotId(node->id(), 2);
                SlotId slotId3 = SlotId(node->id(), 3);

                bool connected;
                std::string outType = GetSlotDataType(graph, slotId0);
                std::string outValue = graph->GetSlotName(slotId0);
                std::string outType1 = GetSlotDataType(graph, slotId1);
                std::string outValue1 = graph->GetSlotName(slotId1);
                

                std::string TextureSamplerY = ResolveSlot(codeGenerateData, slotId2, &connected);
                std::string fac = ResolveSlot(codeGenerateData,  slotId3, &connected);


                codeGenerateData->fragment_stream << " " << outType << " " << outValue <<";\n";
                codeGenerateData->fragment_stream << " " << outType1 << " " << outValue1 <<";\n";
                codeGenerateData->fragment_stream << "valtorgb("<<fac<<","<<"ramp_curve"<<","<<TextureSamplerY<<","<<outValue<<","<<outValue1<<");\n";
                if(getOutIndex == 0){
                    return outValue;
                }else{
                    return outValue1;
                }
            }
                break;
            case NodeType::COMBINE_HSV:
            {

                std::vector<std::string> outParam;
                ProcessNode(codeGenerateData,  node, "node_combine_hsv(%s,%s,%s,%s)",  outParam);
                return outParam[getOutIndex];

            }
                break;
            case NodeType::COMBINE_RGB:
            {
                std::vector<std::string> outParam;
                ProcessNode(codeGenerateData,  node, "node_combine_rgb(%s,%s,%s,%s)",  outParam);
                return outParam[getOutIndex];
            }
                break;
            case NodeType::COMBINE_XYZ:
            {
                std::vector<std::string> outParam;
                ProcessNode(codeGenerateData,  node, "node_combine_xyz(%s,%s,%s,%s)",  outParam);
                return outParam[getOutIndex];
            }
                break;
            case NodeType::MAP_RANGE:
            {
                std::vector<std::string> outParam;
                ProcessNode(codeGenerateData,  node, "(%s,%s,%s,%s,%s,%s,%s)",  outParam,2<<1);
                return outParam[getOutIndex];
            }
                break;
            case NodeType::MATH:
            {
                std::vector<std::string> outParam;
                ProcessNode(codeGenerateData,  node, "(%s,%s,%s,%s)",  outParam,2<<1);
                return outParam[getOutIndex];
            }
                break;
            case NodeType::RGB_TO_BW:
            {
                std::vector<std::string> outParam;
                ProcessNode(codeGenerateData,  node, "node_rgbtobw(%s,%s)",  outParam);
                return outParam[getOutIndex];
            }
                break;
            case NodeType::SEPARATE_HSV:
                {
                    std::vector<std::string> outParam;
                    ProcessNode(codeGenerateData,  node, "node_separate_hsv(%s,%s,%s,%s)", outParam);
                    return outParam[getOutIndex];
                }
            	break;
            case NodeType::SEPARATE_RGB:
                {
                    std::vector<std::string> outParam;
                    ProcessNode(codeGenerateData,  node, "node_separate_rgb(%s,%s,%s,%s)", outParam);
                    return outParam[getOutIndex];
                }
            	break;
            case NodeType::SEPARATE_XYZ:
                {
                    std::vector<std::string> outParam;
                    ProcessNode(codeGenerateData,  node, "node_separate_xyz(%s,%s,%s,%s)", outParam);
                    return outParam[getOutIndex];
                }
            	break;
            case NodeType::VECTOR_MATH:
                {
                    std::vector<std::string> outParam;
                    ProcessNode(codeGenerateData,  node, "(%s,%s,%s,%s,%s,%s)", outParam,2<<2);
                    return outParam[getOutIndex];
                }
            	break;
            case NodeType::WAVELENGTH:
                {
                    SlotId slotId1 = SlotId(node->id(), 1);
                    
                    std::vector<std::string> extInput;
                    codeGenerateData->shareState->setSlotLayer(slotId1.slot_id(), codeGenerateData->current_layer);
                    extInput.push_back(std::to_string(codeGenerateData->current_layer));
                    codeGenerateData->current_layer = SetWaveLength(codeGenerateData->shareState,codeGenerateData->current_layer);
                    std::vector<std::string> outParam;
                    ProcessNode(codeGenerateData,  node, "node_wavelength(%s,%s,%s)",  outParam,0,&extInput);
                    return outParam[getOutIndex];
                }
            	break;
            //	//////
            //	// Input
            //	//////
//            case NodeType::AMBIENT_OCCLUSION:
//            	_slots.push_back(Slot{ "Color",       "color",      SlotDirection::OUTPUT, SlotType::COLOR });
//            	_slots.push_back(Slot{ "AO",          "ao",         SlotDirection::OUTPUT, SlotType::FLOAT });
//            	_slots.push_back(Slot{ "Samples",     "samples",    IntSlotValue{ 16,  1, 128 } });
//            	_slots.push_back(Slot{ "Inside",      "inside",     BoolSlotValue{ false } });
//            	_slots.push_back(Slot{ "Only Local",  "only_local", BoolSlotValue{ false } });
//            	_slots.push_back(Slot{ "Color",       "color",      ColorSlotValue{ csc::Float3{ 1.0f, 1.0f, 1.0f} } });
//            	_slots.push_back(Slot{ "Distance",    "distance",   FloatSlotValue{ 1.0f,  0.0f, FLT_MAX } });
//            	_slots.push_back(Slot{ "Normal",      "normal",     SlotDirection::INPUT, SlotType::VECTOR });
//            	break;
            case NodeType::BEVEL:
                {
                    std::vector<std::string> outParam;
                    ProcessNode(codeGenerateData,  node, "node_bevel(%s,%s,%s,%s)", outParam,2<<2);
                    return outParam[getOutIndex];
                }
            	break;
            case NodeType::CAMERA_DATA:
                {
                    std::vector<std::string> outParam;
                    std::vector<std::string> inputExtra;
                    inputExtra.push_back("GPU_WORLD_POSITION");
                    ProcessNode(codeGenerateData,  node, "node_camera(%s,%s,%s,%s)", outParam,0,&inputExtra);
                    return outParam[getOutIndex];
                }
            	break;
            case NodeType::FRESNEL:
                {
                    std::vector<std::string> outParam;
                    std::vector<std::string> inputExtra;
                    inputExtra.push_back("GPU_VIEW_POSITION");
                    ProcessNode(codeGenerateData,  node, "node_fresnel(%s,%s,%s,%s)", outParam,0,&inputExtra);
                    return outParam[getOutIndex];
                }
            	break;
            case NodeType::GEOMETRY:
                {
                    std::vector<std::string> outParam;
                    std::vector<std::string> inputExtra;
                    inputExtra.push_back("GPU_VIEW_POSITION");
                    inputExtra.push_back("GPU_WORLD_NORMAL");
                    inputExtra.push_back("VERTEX_CD_ORCO");
                    inputExtra.push_back("GPU_BARYCENTRIC_TEXCO");
                    inputExtra.push_back("GPU_WORLD_POSITION");
                    ProcessNode(codeGenerateData,  node, "node_geometry(%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s)", outParam,0,&inputExtra);
                    return outParam[getOutIndex];
                }
            	break;
            case NodeType::LAYER_WEIGHT:
                {
                    std::vector<std::string> outParam;
                    std::vector<std::string> inputExtra;
                    inputExtra.push_back("GPU_VIEW_POSITION");
                    ProcessNode(codeGenerateData,  node, "node_layer_weight(%s,%s,%s,%s,%s)", outParam,0,&inputExtra);
                    return outParam[getOutIndex];
                }
            	break;
            //case NodeType::LIGHT_PATH:
            //	_slots.push_back(Slot{ "Is Camera Ray",       "is_camera_ray",       SlotDirection::OUTPUT, SlotType::FLOAT });
            //	_slots.push_back(Slot{ "Is Shadow Ray",       "is_shadow_ray",       SlotDirection::OUTPUT, SlotType::FLOAT });
            //	_slots.push_back(Slot{ "Is Diffuse Ray",      "is_diffuse_ray",      SlotDirection::OUTPUT, SlotType::FLOAT });
            //	_slots.push_back(Slot{ "Is Glossy Ray",       "is_glossy_ray",       SlotDirection::OUTPUT, SlotType::FLOAT });
            //	_slots.push_back(Slot{ "Is Singular Ray",     "is_singular_ray",     SlotDirection::OUTPUT, SlotType::FLOAT });
            //	_slots.push_back(Slot{ "Is Reflection Ray",   "is_reflection_ray",   SlotDirection::OUTPUT, SlotType::FLOAT });
            //	_slots.push_back(Slot{ "Is Transmission Ray", "is_transmission_ray", SlotDirection::OUTPUT, SlotType::FLOAT });
            //	_slots.push_back(Slot{ "Ray Length",          "ray_length",          SlotDirection::OUTPUT, SlotType::FLOAT });
            //	_slots.push_back(Slot{ "Ray Depth",           "ray_depth",           SlotDirection::OUTPUT, SlotType::FLOAT });
            //	_slots.push_back(Slot{ "Diffuse Depth",       "diffuse_depth",       SlotDirection::OUTPUT, SlotType::FLOAT });
            //	_slots.push_back(Slot{ "Glossy Depth",        "glossy_depth",        SlotDirection::OUTPUT, SlotType::FLOAT });
            //	_slots.push_back(Slot{ "Transparent Depth",   "transparent_depth",   SlotDirection::OUTPUT, SlotType::FLOAT });
            //	_slots.push_back(Slot{ "Transmission Depth",  "transmission_depth",  SlotDirection::OUTPUT, SlotType::FLOAT });
            //	break;
            case NodeType::OBJECT_INFO:
                {
                    std::vector<std::string> outParam;
                    std::vector<std::string> inputExtra;
                    inputExtra.push_back("GPU_OBJECT_COLOR");
                    inputExtra.push_back("GPU_OBJECT_INFO");
                    inputExtra.push_back("GPU_OBJECT_MAT_INDEX");
                    ProcessNode(codeGenerateData,  node, "node_object_info(%s,%s,%s,%s,%s,%s,%s,%s)", outParam,0,&inputExtra);
                    return outParam[getOutIndex];
                }
            	break;
            case NodeType::RGB:
                {
                    std::vector<std::string> outParam;
                    ProcessNode(codeGenerateData,  node, "node_rgb(%s,%s)", outParam);
                    return outParam[getOutIndex];
                                    }
                break;
            case NodeType::TANGENT:
                {
                    std::vector<std::string> outParam;
                    std::vector<std::string> inputExtra;
                    inputExtra.push_back("VERTEX_CD_TANGENT");
                    inputExtra.push_back("VERTEX_CD_ORCO");
                    inputExtra.push_back("GPU_WORLD_NORMAL");
                    ProcessNode(codeGenerateData,  node, "node_tangent(%s,%s,%s,%s,%s,%s)", outParam,0,&inputExtra);
                    return outParam[getOutIndex];
                }
            	break;
            case NodeType::TEXTURE_COORDINATE:
                {
                    std::vector<std::string> outParam;
                    std::vector<std::string> inputExtra;
                    inputExtra.push_back("GPU_VIEW_POSITION");
                    inputExtra.push_back("GPU_WORLD_NORMAL");
                    inputExtra.push_back("GPU_CAMERA_TEXCO_FACTORS");
                    inputExtra.push_back("VERTEX_CD_ORCO");
                    inputExtra.push_back("VERTEX_CD_MTFACE");
                    inputExtra.push_back("GPU_WORLD_POSITION");
                    ProcessNode(codeGenerateData,  node, "node_tex_coord(%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s)", outParam,0,&inputExtra);
                    return outParam[getOutIndex];
                }
            	break;
            case NodeType::VALUE:
                {
                    std::vector<std::string> outParam;
                    ProcessNode(codeGenerateData,  node, "node_value(%s,%s)", outParam);
                    return outParam[getOutIndex];
                }
                break;
            case NodeType::WIREFRAME:
                {
                    std::vector<std::string> outParam;
                    std::vector<std::string> inputExtra;
                    inputExtra.push_back("GPU_BARYCENTRIC_TEXCO");
                    inputExtra.push_back("GPU_BARYCENTRIC_DIST");
                    ProcessNode(codeGenerateData,  node, "node_node_wireframe(%s,%s,%s,%s,%s)", outParam,0,&inputExtra);
                    return outParam[getOutIndex];
                }
            	break;
            //	//////
            //	// Shader
            //	//////
            case NodeType::ADD_SHADER:
                {
                    std::vector<std::string> outParam;
                    ProcessNode(codeGenerateData,  node, "node_add_shader(%s,%s,%s)", outParam);
                    return outParam[getOutIndex];
                }
            	break;
            //case NodeType::ANISOTROPIC_BSDF:
            //	_slots.push_back(Slot{ "BSDF",         "BSDF",         SlotDirection::OUTPUT, SlotType::CLOSURE });
            //	_slots.push_back(Slot{ "Distribution", "distribution", EnumSlotValue{ AnisotropicDistribution::GGX } });
            //	_slots.push_back(Slot{ "Color",        "color",        ColorSlotValue{ csc::Float3{ 0.9f, 0.9f, 0.9f} } });
            //	_slots.push_back(Slot{ "Roughness",    "roughness",    FloatSlotValue{ 0.0f,  0.0f, 1.0f } });
            //	_slots.push_back(Slot{ "Anisotropy",   "anisotropy",   FloatSlotValue{ 0.5f, -1.0f, 1.0f } });
            //	_slots.push_back(Slot{ "Rotation",     "rotation",     FloatSlotValue{ 0.0f,  0.0f, 1.0f } });
            //	_slots.push_back(Slot{ "Normal",       "normal",       SlotDirection::INPUT, SlotType::VECTOR });
            //	_slots.push_back(Slot{ "Tangent",      "tangent",      SlotDirection::INPUT, SlotType::VECTOR });
            //	break;
            case NodeType::DIFFUSE_BSDF:
                {
                    std::vector<std::string> outParam;
                    std::vector<std::string> extInput;
                    extInput.push_back("GPU_WORLD_POSITION");
                    ProcessNode(codeGenerateData,  node, "node_bsdf_orenNayar(%s,%s,%s,%s,%s)", outParam,0,&extInput);
                    return outParam[getOutIndex];
                }
                break;
            case NodeType::EMISSION:
            {
                std::vector<std::string> outParam;
                ProcessNode(codeGenerateData,  node, "node_bsdf_emission(%s,%s,%s)", outParam);
                return outParam[getOutIndex];
            }
                break;
            //case NodeType::GLASS_BSDF:
            //	_slots.push_back(Slot{ "BSDF",         "BSDF",         SlotDirection::OUTPUT, SlotType::CLOSURE });
            //	_slots.push_back(Slot{ "Distribution", "distribution", EnumSlotValue{ GlassDistribution::GGX } });
            //	_slots.push_back(Slot{ "Color",        "color",        ColorSlotValue{ csc::Float3{ 0.9f, 0.9f, 0.9f} } });
            //	_slots.push_back(Slot{ "Roughness",    "roughness",    FloatSlotValue{ 0.0f, 0.0f, 1.0f } });
            //	_slots.push_back(Slot{ "IOR",          "IOR",          FloatSlotValue{ 1.45f, 0.0f, 100.0f } });
            //	_slots.push_back(Slot{ "Normal",       "normal",       SlotDirection::INPUT, SlotType::VECTOR });
            //	break;
            //case NodeType::GLOSSY_BSDF:
            //	_slots.push_back(Slot{ "BSDF",         "BSDF",         SlotDirection::OUTPUT, SlotType::CLOSURE });
            //	_slots.push_back(Slot{ "Distribution", "distribution", EnumSlotValue{ GlossyDistribution::GGX } });
            //	_slots.push_back(Slot{ "Color",        "color",        ColorSlotValue{ csc::Float3{ 0.9f, 0.9f, 0.9f} } });
            //	_slots.push_back(Slot{ "Roughness",    "roughness",    FloatSlotValue{ 0.0f, 0.0f, 1.0f } });
            //	_slots.push_back(Slot{ "Normal",       "normal",       SlotDirection::INPUT, SlotType::VECTOR });
            //	break;
            case NodeType::PBR:
                {
                    std::vector<std::string> outParam;
                    std::vector<std::string> extInput;
                    extInput.push_back("GPU_WORLD_POSITION");
                    ProcessNode(codeGenerateData,  node, "node_bsdf_pbr(%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s)", outParam,0,&extInput);
                    return outParam[getOutIndex];
                }
                break;
            //case NodeType::HAIR_BSDF:
            //	_slots.push_back(Slot{ "BSDF",       "BSDF",        SlotDirection::OUTPUT, SlotType::CLOSURE });
            //	_slots.push_back(Slot{ "Component",  "component",   EnumSlotValue{ HairComponent::REFLECTION } });
            //	_slots.push_back(Slot{ "Color",      "color",       ColorSlotValue{ csc::Float3{ 0.9f, 0.9f, 0.9f} } });
            //	_slots.push_back(Slot{ "Offset",     "offset",      FloatSlotValue{ 0.0f, -90.0f, 90.0f, 2 } });
            //	_slots.push_back(Slot{ "RoughnessU", "roughness_u", FloatSlotValue{ 0.1f, 0.0f, 1.0f } });
            //	_slots.push_back(Slot{ "RoughnessV", "roughness_v", FloatSlotValue{ 1.0f, 0.0f, 1.0f } });
            //	_slots.push_back(Slot{ "Tangent",    "tangent",     SlotDirection::INPUT, SlotType::VECTOR });
            //	break;
            //case NodeType::HOLDOUT:
            //	_slots.push_back(Slot{ "Holdout", "holdout", SlotDirection::OUTPUT, SlotType::CLOSURE });
            //	break;
            case NodeType::MIX_SHADER:
                {
                    std::vector<std::string> outParam;
                    ProcessNode(codeGenerateData,  node, "node_mix_shader(%s,%s,%s,%s)", outParam);
                    return outParam[getOutIndex];
                }
            	break;
            //case NodeType::PRINCIPLED_BSDF:
            //	_slots.push_back(Slot{ "BSDF",                "BSDF",                 SlotDirection::OUTPUT, SlotType::CLOSURE });
            //	_slots.push_back(Slot{ "Distribution",        "distribution",         EnumSlotValue{ PrincipledBSDFDistribution::GGX } });
            //	_slots.push_back(Slot{ "Base Color",          "base_color",           ColorSlotValue{ csc::Float3{ 0.8f, 0.8f, 0.8f} } });
            //	_slots.push_back(Slot{ "Subsurface Method",   "subsurface_method",    EnumSlotValue{ PrincipledBSDFSubsurfaceMethod::BURLEY } });
            //	_slots.push_back(Slot{ "Subsurface",          "subsurface",           FloatSlotValue{ 0.0f, 0.0f, 1.0f } });
            //	_slots.push_back(Slot{ "Subsurface Radius",   "subsurface_radius",    VectorSlotValue{
            //		csc::Float3{ 1.0f, 0.2f, 0.1f }, csc::Float3{ 0.0f, 0.0f, 0.0f } , csc::Float3{ FLT_MAX, FLT_MAX, FLT_MAX }
            //	} });
            //	_slots.push_back(Slot{ "Subsurface Color",       "subsurface_color",       ColorSlotValue{ csc::Float3{ 0.7f, 1.0f, 1.0f} } });
            //	_slots.push_back(Slot{ "Metallic",               "metallic",               FloatSlotValue{ 0.0f, 0.0f, 1.0f } });
            //	_slots.push_back(Slot{ "Specular",               "specular",               FloatSlotValue{ 0.5f, 0.0f, 1.0f } });
            //	_slots.push_back(Slot{ "Specular Tint",          "specular_tint",          FloatSlotValue{ 0.0f, 0.0f, 1.0f } });
            //	_slots.push_back(Slot{ "Roughness",              "roughness",              FloatSlotValue{ 0.5f, 0.0f, 1.0f } });
            //	_slots.push_back(Slot{ "Anisotropic",            "anisotropic",            FloatSlotValue{ 0.0f, 0.0f, 1.0f } });
            //	_slots.push_back(Slot{ "Anisotropic Rotation",   "anisotropic_rotation",   FloatSlotValue{ 0.0f, 0.0f, 1.0f } });
            //	_slots.push_back(Slot{ "Sheen",                  "sheen",                  FloatSlotValue{ 0.0f, 0.0f, 1.0f } });
            //	_slots.push_back(Slot{ "Sheen Tint",             "sheen_tint",             FloatSlotValue{ 0.5f, 0.0f, 1.0f } });
            //	_slots.push_back(Slot{ "Clearcoat",              "clearcoat",              FloatSlotValue{ 0.0f, 0.0f, 1.0f } });
            //	_slots.push_back(Slot{ "Clearcoat Roughness",    "clearcoat_roughness",    FloatSlotValue{ 0.0f, 0.0f, 1.0f } });
            //	_slots.push_back(Slot{ "IOR",                    "ior",                    FloatSlotValue{ 1.45f, 0.0f, 100.0f } });
            //	_slots.push_back(Slot{ "Transmission",           "transmission",           FloatSlotValue{ 0.0f, 0.0f, 1.0f } });
            //	_slots.push_back(Slot{ "Transmission Roughness", "transmission_roughness", FloatSlotValue{ 0.0f, 0.0f, 1.0f } });
            //	_slots.push_back(Slot{ "Emission",               "emission",               ColorSlotValue{ csc::Float3{ 0.0f, 0.0f, 0.0f } } });
            //	_slots.push_back(Slot{ "Emission Strength",      "emission_strength",      FloatSlotValue{ 1.0f, 0.0f, 10000.0f } });
            //	_slots.push_back(Slot{ "Alpha",                  "alpha",                  FloatSlotValue{ 1.0f, 0.0f, 1.0f } });
            //	_slots.push_back(Slot{ "Normal",                 "normal",                 SlotDirection::INPUT, SlotType::VECTOR });
            //	_slots.push_back(Slot{ "Clearcoat Normal",       "clearcoat_normal",       SlotDirection::INPUT, SlotType::VECTOR });
            //	_slots.push_back(Slot{ "Tangent",                "tangent",                SlotDirection::INPUT, SlotType::VECTOR });
            //	break;
            //case NodeType::PRINCIPLED_HAIR:
            //	_slots.push_back(Slot{ "BSDF",                   "BSDF",                   SlotDirection::OUTPUT, SlotType::CLOSURE });
            //	_slots.push_back(Slot{ "Coloring",               "coloring",               EnumSlotValue{ PrincipledHairColoring::DIRECT_COLORING } });
            //	_slots.push_back(Slot{ "Color",                  "color",                  ColorSlotValue{ csc::Float3{ 0.017513f, 0.005763f, 0.002059f } } });
            //	_slots.push_back(Slot{ "Melanin",                "melanin",                FloatSlotValue{ 0.8f, 0.0f, 1.0f } });
            //	_slots.push_back(Slot{ "Melanin Redness",        "melanin_redness",        FloatSlotValue{ 1.0f, 0.0f, 1.0f } });
            //	_slots.push_back(Slot{ "Tint",                   "tint",                   ColorSlotValue{ csc::Float3{ 1.0f, 1.0f, 1.0f } } });
            //	_slots.push_back(Slot{ "Absorption Coefficient", "absorption_coefficient", VectorSlotValue{
            //		csc::Float3{ 0.245531f, 0.52f, 1.365f }, csc::Float3{ 0.0f, 0.0f, 0.0f } , csc::Float3{ FLT_MAX, FLT_MAX, FLT_MAX }
            //	} });
            //	_slots.push_back(Slot{ "Roughness",              "roughness",              FloatSlotValue{ 0.3f, 0.0f, 1.0f } });
            //	_slots.push_back(Slot{ "Radial Roughness",       "radial_roughness",       FloatSlotValue{ 0.3f, 0.0f, 1.0f } });
            //	_slots.push_back(Slot{ "Coat",                   "coat",                   FloatSlotValue{ 0.0f, 0.0f, 1.0f } });
            //	_slots.push_back(Slot{ "IOR",                    "ior",                    FloatSlotValue{ 1.55f, 0.0f, 1000.0f } });
            //	_slots.push_back(Slot{ "Offset",                 "offset",                 FloatSlotValue{ 2 * MY_PI / 180.0f, MY_PI / -2.0f , MY_PI / 2.0f } });
            //	_slots.push_back(Slot{ "Random Roughness",       "random_roughness",       FloatSlotValue{ 0.0f, 0.0f, 1.0f } });
            //	_slots.push_back(Slot{ "Random Color",           "random_color",           FloatSlotValue{ 0.0f, 0.0f, 1.0f } });
            //	_slots.push_back(Slot{ "Random",                 "random",                 FloatSlotValue{ 0.0f, 0.0f, FLT_MAX } });
            //	break;
            //case NodeType::PRINCIPLED_VOLUME:
            //	_slots.push_back(Slot{ "Volume",              "volume",              SlotDirection::OUTPUT, SlotType::CLOSURE });
            //	_slots.push_back(Slot{ "Color",               "color",               ColorSlotValue{ csc::Float3{ 0.5f, 0.5f, 0.5f } } });
            //	_slots.push_back(Slot{ "Density",             "density",             FloatSlotValue{ 1.0f, 0.0f, FLT_MAX } });
            //	_slots.push_back(Slot{ "Anisotropy",          "anisotropy",          FloatSlotValue{ 0.0f, -1.0f, 1.0f } });
            //	_slots.push_back(Slot{ "Absorption Color",    "absorption_color",    ColorSlotValue{ csc::Float3{ 0.0f, 0.0f, 0.0f } } });
            //	_slots.push_back(Slot{ "Emission Strength",   "emission_strength",   FloatSlotValue{ 0.0f, 0.0f, FLT_MAX } });
            //	_slots.push_back(Slot{ "Emission Color",      "emission_color",      ColorSlotValue{ csc::Float3{ 1.0f, 1.0f, 1.0f } } });
            //	_slots.push_back(Slot{ "Blackbody Intensity", "blackbody_intensity", FloatSlotValue{ 0.0f, 0.0f, 1.0f } });
            //	_slots.push_back(Slot{ "Blackbody Tint",      "blackbody_tint",      ColorSlotValue{ csc::Float3{ 1.0f, 1.0f, 1.0f } } });
            //	_slots.push_back(Slot{ "Temperature",         "temperature",         FloatSlotValue{ 1000.0f, 0.0f, 8000.0f } });
            //	break;
            //case NodeType::REFRACTION_BSDF:
            //	_slots.push_back(Slot{ "BSDF",         "BSDF",         SlotDirection::OUTPUT, SlotType::CLOSURE });
            //	_slots.push_back(Slot{ "Distribution", "distribution", EnumSlotValue{ RefractionDistribution::GGX } });
            //	_slots.push_back(Slot{ "Color",        "color",        ColorSlotValue{ csc::Float3{ 0.9f, 0.9f, 0.9f} } });
            //	_slots.push_back(Slot{ "Roughness",    "roughness",    FloatSlotValue{ 0.0f, 0.0f, 1.0f } });
            //	_slots.push_back(Slot{ "IOR",          "IOR",          FloatSlotValue{ 1.45f, 0.0f, 100.0f } });
            //	_slots.push_back(Slot{ "Normal",       "normal",       SlotDirection::INPUT, SlotType::VECTOR });
            //	break;
            //case NodeType::SUBSURFACE_SCATTER:
            //	_slots.push_back(Slot{ "BSSRDF",       "BSSRDF",       SlotDirection::OUTPUT, SlotType::CLOSURE });
            //	_slots.push_back(Slot{ "Falloff",      "falloff",      EnumSlotValue{ SubsurfaceScatterFalloff::BURLEY } });
            //	_slots.push_back(Slot{ "Color",        "color",        ColorSlotValue{ csc::Float3{ 0.9f, 0.9f, 0.9f} } });
            //	_slots.push_back(Slot{ "Scale",        "scale",        FloatSlotValue{ 1.0f, 0.0f, FLT_MAX } });
            //	_slots.push_back(Slot{ "Radius",       "radius",       VectorSlotValue{
            //		csc::Float3{ 1.0f, 1.0f, 1.0f }, csc::Float3{ 0.0f, 0.0f, 0.0f } , csc::Float3{ FLT_MAX, FLT_MAX, FLT_MAX }
            //	} });
            //	_slots.push_back(Slot{ "Sharpness",    "sharpness",    FloatSlotValue{ 0.0f, 0.0f, 1.0f } });
            //	_slots.push_back(Slot{ "Texture Blur", "texture_blur", FloatSlotValue{ 0.0f, 0.0f, 1.0f } });
            //	_slots.push_back(Slot{ "Normal",       "normal",       SlotDirection::INPUT, SlotType::VECTOR });
            //	break;
            //case NodeType::TOON_BSDF:
                //_slots.push_back(Slot{ "BSDF",      "BSDF",      SlotDirection::OUTPUT, SlotType::CLOSURE });
                //_slots.push_back(Slot{ "Component", "component", EnumSlotValue{ ToonComponent::DIFFUSE } });
                //_slots.push_back(Slot{ "Color",     "color",     ColorSlotValue{ csc::Float3{ 0.9f, 0.9f, 0.9f} } });
                //_slots.push_back(Slot{ "Size",      "size",      FloatSlotValue{ 0.5f, 0.0f, 1.0f } });
                //_slots.push_back(Slot{ "Smooth",    "smooth",    FloatSlotValue{ 0.0f, 0.0f, 1.0f } });
                //_slots.push_back(Slot{ "Normal",    "normal",    SlotDirection::INPUT, SlotType::VECTOR });
                //break;
            //case NodeType::TRANSLUCENT_BSDF:
            //	_slots.push_back(Slot{ "BSDF",   "BSDF",   SlotDirection::OUTPUT, SlotType::CLOSURE });
            //	_slots.push_back(Slot{ "Color",  "color",  ColorSlotValue{ csc::Float3{ 0.9f, 0.9f, 0.9f} } });
            //	_slots.push_back(Slot{ "Normal", "normal", SlotDirection::INPUT, SlotType::VECTOR });
            //	break;
        //	case NodeType::TRANSPARENT_BSDF:
        ////		_slots.push_back(Slot{ "BSDF",  "BSDF",  SlotDirection::OUTPUT, SlotType::CLOSURE });
        ////		_slots.push_back(Slot{ "Color", "color", ColorSlotValue{ csc::Float3{ 0.9f, 0.9f, 0.9f} } });
        ////        {
        ////
        ////        }
        //		break;
            //case NodeType::VELVET_BSDF:
            //	_slots.push_back(Slot{ "BSDF",   "BSDF",   SlotDirection::OUTPUT, SlotType::CLOSURE });
            //	_slots.push_back(Slot{ "Color",  "color",  ColorSlotValue{ csc::Float3{ 0.9f, 0.9f, 0.9f} } });
            //	_slots.push_back(Slot{ "Sigma",  "sigma",  FloatSlotValue{ 1.0f, 0.0f, 1.0f } });
            //	_slots.push_back(Slot{ "Normal", "normal", SlotDirection::INPUT, SlotType::VECTOR });
            //	break;
            //case NodeType::VOL_ABSORPTION:
            //	_slots.push_back(Slot{ "Volume",  "volume",  SlotDirection::OUTPUT, SlotType::CLOSURE });
            //	_slots.push_back(Slot{ "Color",   "color",   ColorSlotValue{ csc::Float3{ 0.9f, 0.9f, 0.9f} } });
            //	_slots.push_back(Slot{ "Density", "density", FloatSlotValue{ 1.0f, 0.0f, FLT_MAX } });
            //	break;
            //case NodeType::VOL_SCATTER:
            //	_slots.push_back(Slot{ "Volume",     "volume",     SlotDirection::OUTPUT, SlotType::CLOSURE });
            //	_slots.push_back(Slot{ "Color",      "color",      ColorSlotValue{ csc::Float3{ 0.9f, 0.9f, 0.9f} } });
            //	_slots.push_back(Slot{ "Density",    "density",    FloatSlotValue{ 1.0f, 0.0f, FLT_MAX } });
            //	_slots.push_back(Slot{ "Anisotropy", "anisotropy", FloatSlotValue{ 0.0f, -1.0f, 1.0f } });
            //	break;
            //	//////
            //	// Texture
            //	//////
            case NodeType::BRICK_TEX:
                {
                    std::vector<std::string> outParam;
                    ProcessNode(codeGenerateData,  node, "node_tex_brick(%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s)", outParam);
                    return outParam[getOutIndex];
                }
            	break;
            case NodeType::CHECKER_TEX:
                {
                    std::vector<std::string> outParam;
                    ProcessNode(codeGenerateData,  node, "node_tex_checker(%s,%s,%s,%s,%s,%s)", outParam);
                    return outParam[getOutIndex];
                }
            	break;
            case NodeType::GRADIENT_TEX:
                {
                    std::vector<std::string> outParam;
                    ProcessNode(codeGenerateData,  node, "(%s,%s,%s)", outParam,2<<2);
                    return outParam[getOutIndex];
                }
            	break;
            case NodeType::IMAGE_TEX:
                {
                    std::vector<std::string> outParam;
                    ProcessNode(codeGenerateData,  node, "(%s,%s,%s,%s,%s)", outParam,2<<2);
                    return outParam[getOutIndex];
                }
                break;
            case NodeType::MAGIC_TEX:
                {
                   std::vector<std::string> outParam;
                   ProcessNode(codeGenerateData,  node, "node_tex_magic(%s,%s,%s,%s,%s,%s)", outParam);
                   return outParam[getOutIndex];
                }
            	break;
            case NodeType::MUSGRAVE_TEX:
                {
                    std::vector<std::string> outParam;
                    ProcessNode(codeGenerateData,  node, "(%s,%s,%s,%s,%s,%s,%s,%s,%s)", outParam,2<<1|2<<2);
                    return outParam[getOutIndex];
                }
            	break;
            case NodeType::NOISE_TEX:
                {
                    std::vector<std::string> outParam;
                   ProcessNode(codeGenerateData,  node, "(%s,%s,%s,%s,%s,%s,%s,%s)", outParam,2<<2);
                   return outParam[getOutIndex];
                }
            	break;
            case NodeType::VORONOI_TEX:
                {
                    std::vector<std::string> outParam;
                    ProcessNode(codeGenerateData,  node, "_node_tex_voronoi(%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s)", outParam,2<<5|2<<6);
                    return outParam[getOutIndex];
                }
            	break;
            case NodeType::WAVE_TEX:
                {
                    std::vector<std::string> outParam;
                    ProcessNode(codeGenerateData,  node, "node_tex_wave(%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s)", outParam);
                    return outParam[getOutIndex];
                }
            	break;
            case NodeType::WHITE_NOISE_TEX:
                {
                   std::vector<std::string> outParam;
                    ProcessNode(codeGenerateData,  node, "(%s,%s,%s,%s)", outParam,2<<2);
                    return outParam[getOutIndex];
                }
            	break;
            //	//////
            //	// Vector
            //	//////
            case NodeType::BUMP:
                {
                    std::vector<std::string> outParam;
                    std::vector<std::string> extInput;
                    extInput.push_back("GPU_VIEW_POSITION");
                    ProcessNode(codeGenerateData,  node, "node_bump(%s,%s,%s,%s,%s,%s,%s)", outParam,0,&extInput);
                    return outParam[getOutIndex];
                }
            	break;
            case NodeType::DISPLACEMENT:
                {
                    std::vector<std::string> outParam;
                    ProcessNode(codeGenerateData,  node, "(%s,%s,%s,%s,%s)", outParam,2<<1);
                    return outParam[getOutIndex];
                }
            	break;
            case NodeType::MAPPING:
            //	_slots.push_back(Slot{ "Vector",   "vector",       SlotDirection::OUTPUT, SlotType::VECTOR });
//            	_slots.push_back(Slot{ "Type",     "mapping_type", EnumSlotValue{ VectorMappingType::POINT } });
            //	_slots.push_back(Slot{ "Vector",   "vector",       VectorSlotValue{
            //		csc::Float3{ 0.0f, 0.0f, 0.0f }, csc::Float3{ -FLT_MAX, -FLT_MAX, -FLT_MAX } , csc::Float3{ FLT_MAX, FLT_MAX, FLT_MAX }
            //	} });
            //	_slots.push_back(Slot{ "Location", "location",     VectorSlotValue{
            //		csc::Float3{ 0.0f, 0.0f, 0.0f }, csc::Float3{ -FLT_MAX, -FLT_MAX, -FLT_MAX } , csc::Float3{ FLT_MAX, FLT_MAX, FLT_MAX }
            //	} });
            //	_slots.push_back(Slot{ "Rotation", "rotation",     VectorSlotValue{
            //		csc::Float3{ 0.0f, 0.0f, 0.0f }, csc::Float3{ -FLT_MAX, -FLT_MAX, -FLT_MAX } , csc::Float3{ FLT_MAX, FLT_MAX, FLT_MAX }
            //	} });
            //	_slots.push_back(Slot{ "Scale", "scale",           VectorSlotValue{
            //		csc::Float3{ 1.0f, 1.0f, 1.0f }, csc::Float3{ -FLT_MAX, -FLT_MAX, -FLT_MAX } , csc::Float3{ FLT_MAX, FLT_MAX, FLT_MAX }
            //	} });
            //	_slot_aliases.push_back(std::make_pair("type", "mapping_type"));
                {
                    std::vector<std::string> outParam;
                    ProcessNode(codeGenerateData,  node, "(%s,%s,%s,%s,%s)", outParam,2<<1);
                    return outParam[getOutIndex];
                }
            	break;
            case NodeType::NORMAL:
            //	_slots.push_back(Slot{ "Normal",    "normal",    SlotDirection::OUTPUT, SlotType::VECTOR });
            //	_slots.push_back(Slot{ "Dot",       "dot",       SlotDirection::OUTPUT, SlotType::FLOAT });
            //	_slots.push_back(Slot{ "Direction", "direction", VectorSlotValue{
            //		csc::Float3{ 0.0f, 0.0f, 0.0f },  csc::Float3{ -FLT_MAX, -FLT_MAX, -FLT_MAX } , csc::Float3{ FLT_MAX, FLT_MAX, FLT_MAX }
            //	}, false });
            //	_slots.push_back(Slot{ "Normal",    "normal",    VectorSlotValue{
            //		csc::Float3{ 0.0f, 0.0f, 0.0f }, csc::Float3{ -FLT_MAX, -FLT_MAX, -FLT_MAX } , csc::Float3{ FLT_MAX, FLT_MAX, FLT_MAX }
            //	} });
                {
                    std::vector<std::string> outParam;
                    ProcessNode(codeGenerateData,  node, "node_normal(%s,%s,%s,%s)", outParam);
                    return outParam[getOutIndex];
                }
            	break;
            case NodeType::NORMAL_MAP:
            //	_slots.push_back(Slot{ "Normal",   "normal",   SlotDirection::OUTPUT, SlotType::VECTOR });
//            	_slots.push_back(Slot{ "Space",    "space",    EnumSlotValue{ NormalMapSpace::TANGENT } });
            //	_slots.push_back(Slot{ "Strength", "strength", FloatSlotValue{ 1.0f, 0.0f, 10.0f } });
            //	_slots.push_back(Slot{ "Color",    "color",    ColorSlotValue{ csc::Float3{ 0.5f, 0.5f, 1.0f} } });
                {
                    std::vector<std::string> outParam;
                    std::vector<std::string> inputExtra;
                    inputExtra.push_back("GPU_OBJECT_INFO");
                    inputExtra.push_back("VERTEX_CD_TANGENT");
                    inputExtra.push_back("GPU_WORLD_NORMAL");
                    ProcessNode(codeGenerateData,  node, "(%s,%s,%s,%s,%s,%s)", outParam,2<<1,&inputExtra);
                    return outParam[getOutIndex];
                }
            	break;
            case NodeType::VECTOR_CURVES:
//            	_slots.push_back(Slot{ "Vector", "vector", SlotDirection::OUTPUT, SlotType::VECTOR });
//            	_slots.push_back(Slot{ "Curves", "curves", VectorCurveSlotValue{ csc::Float2{ -1.0f, -1.0f }, csc::Float2{ 1.0f, 1.0f} } });
//            	_slots.push_back(Slot{ "Fac",    "fac",    FloatSlotValue{ 1.0f, 0.0f, 1.0f } });
//            	_slots.push_back(Slot{ "Vector", "vector", VectorSlotValue{
//            		csc::Float3{ 0.0f, 0.0f, 0.0f }, csc::Float3{ -FLT_MAX, -FLT_MAX, -FLT_MAX } , csc::Float3{ FLT_MAX, FLT_MAX, FLT_MAX }
//            	} });
                {
                    SlotId slotId0 = SlotId(node->id(), 1);
                    std::string outValue = graph->GetSlotName(slotId0);
                    
                    std::vector<std::string> extInput;
                    extInput.push_back(outValue+"_range");
                    extInput.push_back(outValue+"_ext_x");
                    extInput.push_back(outValue+"_ext_y");
                    extInput.push_back(outValue+"_ext_z");
                    std::vector<std::string> outParam;
                    ProcessNode(codeGenerateData,  node, "node_curves_vec(%s,%s,%s,%s,%s,%s,%s,%s)",outParam,0,&extInput);
                    return outParam[getOutIndex];
                }
            	break;
            case NodeType::VECTOR_DISPLACEMENT:
            //	_slots.push_back(Slot{ "Displacement", "displacement", SlotDirection::OUTPUT, SlotType::VECTOR });
//            	_slots.push_back(Slot{ "Space",        "space",        EnumSlotValue{ VectorDisplacementSpace::TANGENT } });
            //	_slots.push_back(Slot{ "Vector",       "vector",       SlotDirection::INPUT, SlotType::COLOR });
            //	_slots.push_back(Slot{ "Midlevel",     "midlevel",     FloatSlotValue{ 0.0f, 0.0f, FLT_MAX } });
            //	_slots.push_back(Slot{ "Scale",        "scale",        FloatSlotValue{ 1.0f, 0.0f, FLT_MAX } });
                {
                    std::vector<std::string> outParam;
                    std::vector<std::string> inputExtra;
                    inputExtra.push_back("VERTEX_CD_TANGENT");
                    inputExtra.push_back("GPU_WORLD_NORMAL");
                    ProcessNode(codeGenerateData,  node, "(%s,%s,%s,%s,%s,%s)", outParam,2<<1,&inputExtra);
                    return outParam[getOutIndex];
                }
            	break;
            case NodeType::VECTOR_TRANSFORM:
                {
                    std::vector<std::string> outParam;
                    ProcessNode(codeGenerateData,  node, "(%s,%s)", outParam,2<<1|2<<2|2<<3);
                    return outParam[getOutIndex];
                }
            	break;
            default:
                // Uncomment the below assert once all node types have been implemented
                assert(false);
            }

        }

        std::shared_ptr<csg::CodeGenerateData> generate_graph_code( std::shared_ptr<Graph> graph,std::shared_ptr<cse::SharedState> sharedState){

            std::shared_ptr<csg::CodeGenerateData> codeGenerateData = std::make_shared<csg::CodeGenerateData>(graph,sharedState) ;
            std::shared_ptr<Node> masterNode;
            for (const auto& node : graph->nodes()) {
                if (node->type() == csg::NodeType::MATERIAL_OUTPUT) {
                    masterNode = node;
                    break;
                }
            }
                        
            AddPosAttribute(codeGenerateData);
            AddNormalAttribute(codeGenerateData);
            AddLightUniform(codeGenerateData);

            AddMVPUniform(codeGenerateData);


            ResolveNode(codeGenerateData,masterNode);
            

            return codeGenerateData;
        }

        bool complie_graph( std::shared_ptr<Graph> the_graph,std::shared_ptr<cse::SharedState> shared_state,bool saveData2File){
            std::shared_ptr<csg::CodeGenerateData> code = csg::generate_graph_code(the_graph,shared_state);
            shared_state->record_connect_nodes(code->get_connected_nodes());
            auto uniformParams = code->GetUniformData(shared_state);
            uint32_t size = (uint32_t)uniformParams.size()*sizeof(csc::UniformData);
            if(saveData2File){
                std::string str = the_graph->serialize();
                shared_state->set_output_code(str,code->ExportVertex(), code->ExportFragment(),code->ExportVaring(),(void*)&uniformParams[0],size);
            }else{
                shared_state->set_output_code("",code->ExportVertex(), code->ExportFragment(),code->ExportVaring(),(void*)&uniformParams[0],size);
            }
            return true;
        }

}
