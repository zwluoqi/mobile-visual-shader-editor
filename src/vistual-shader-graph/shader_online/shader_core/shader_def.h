#pragma once

/**
 * @file
 * @brief Defines Float2, Float3, and Int2.
 */

#include <array>
#include <cmath>
#include <cstddef>
#include <map>
#include <string>

#include "vector.h"
#include "bgfx_utils.h"

namespace csc {


    #define MAX_COLOR_BAND 256
    #define CM_TABLE 32

    struct ShaderDataType{
        enum  Enum{
                attribute,//vertex input from obj
                uniformVertex,//vertex input from app
                uniformFrag,//fragment input from app
                varying,//fragment input from vertex
                cout
            };
    };

    struct ShaderAttributeType{
        enum Enum{
            Position,  //!< a_position
            Normal,    //!< a_normal
            Tangent,   //!< a_tangent
            Bitangent, //!< a_bitangent
            Color0,    //!< a_color0
            Color1,    //!< a_color1
            Color2,    //!< a_color2
            Color3,    //!< a_color3
            Indices,   //!< a_indices
            Weight,    //!< a_weight
            TexCoord0, //!< a_texcoord0
            TexCoord1, //!< a_texcoord1
            TexCoord2, //!< a_texcoord2
            TexCoord3, //!< a_texcoord3
            TexCoord4, //!< a_texcoord4
            TexCoord5, //!< a_texcoord5
            TexCoord6, //!< a_texcoord6
            TexCoord7, //!< a_texcoord7

            Count
        };
    };

    struct UniformType{
        enum Enum{
                Sampler, //!< Sampler.
                End,     //!< Reserved, do not use.

                Vec4,    //!< 4 floats vector.
                Mat3,    //!< 3x3 matrix.
                Mat4,    //!< 4x4 matrix.
                
                Vec3,    //!<<same vec4
                Vec2,    //!<<same vec4
                Float,    //!<<same vec4
                Count
        };
    };

    struct TextureData{
        bgfx::TextureHandle handle;//本地存储的全局采样ID
        uint16_t samplerHandleIndex;
    };

    union UniformUnionData{
          TextureData defaultTextureData;
          Float4 defaultValue;
          UniformUnionData(){}
          UniformUnionData(const Float4& f4):defaultValue{f4}{}
          UniformUnionData(const TextureData& f4):defaultTextureData{f4}{}
          //mat3 mat4 暂时不支持
   };
    struct UniformData {
        uint64_t uniformId;
        UniformType::Enum uniformType;
        UniformUnionData uniformUnionData;
        char uniformName[64];
    };

    struct UniformChangeData{
        uint64_t uniformId;
        UniformUnionData uniformUnionData;
        UniformChangeData(uint64_t _id,const Float4 f4):uniformId{_id},uniformUnionData{f4}{}
        UniformChangeData(uint64_t _id,const TextureData f4):uniformId{_id},uniformUnionData{f4}{}
        UniformChangeData(){
            
        }
    };

    struct GraphMaterialTextureData{
        bgfx::TextureHandle handle;//本地存储的全局采样ID
        bgfx::TextureInfo textureInfo;
    };

    struct MaterialShaderContext{
        
        MaterialShaderContext(){
            m_curveRampTexture.handle =
                        bgfx::createTexture2D(
              MAX_COLOR_BAND
            , CM_TABLE + 1
            , false
            , 1
            , bgfx::TextureFormat::RGBA8
            , BGFX_TEXTURE_NONE
                                  );

            auto _info = &m_curveRampTexture.textureInfo;
            _info->format  = bgfx::TextureFormat::RGBA8;
            _info->width   = MAX_COLOR_BAND;
            _info->height  = CM_TABLE + 1;
            
            m_curveRampUniformId = 1;
//            _info->depth   = _depth;
//            _info->numMips = numMips;
//            _info->numLayers = _numLayers;
//            _info->cubeMap   = _cubeMap;
//            _info->storageSize  = size;
//            _info->bitsPerPixel = bpp;
        }
        
        void UpdateLayerRampBuffer(uint32_t* source,int layer){
            uint32_t* dest = m_curveRampBuffer[layer];
            memcpy(dest, source, sizeof(uint32_t) * (MAX_COLOR_BAND));

            bgfx::updateTexture2D(
                                 m_curveRampTexture.handle
                               , 0
                               , 0
                               , 0
                               , 0
                               , MAX_COLOR_BAND
                               , CM_TABLE + 1
                               , bgfx::makeRef(m_curveRampBuffer, (MAX_COLOR_BAND)*(CM_TABLE + 1)*sizeof(uint32_t))
                               );
            
        }
        
        //图片映射采样UID
        std::map<std::string,std::shared_ptr<GraphMaterialTextureData>> fileImage2SamplerUids;
        //默认贴图
        GraphMaterialTextureData m_curveRampTexture;
        uint64_t m_curveRampUniformId;
        uint32_t m_curveRampBuffer[CM_TABLE + 1 ][MAX_COLOR_BAND];
        
        std::map<uint64_t,uint16_t> slot2Layers;
        
        void SetSlotLayer(uint64_t slotId,uint16_t layer){
            slot2Layers[slotId] = layer;
        }
        
        uint16_t GetSlotLayer(uint64_t slotId){
            if(slot2Layers.count(slotId) ==0){
                return 0;
            }else{
                return slot2Layers.at(slotId);
            }
        }
        
        
        bgfx::TextureHandle  GetRampTextureHandler(){
            return m_curveRampTexture.handle;
        }
        
        uint64_t GetRampTextureUniformId(){
            return m_curveRampUniformId;
        }

        bgfx::TextureHandle addTextureHandle(const char* image_path,bgfx::TextureInfo** textureInfo = nullptr){
            std::shared_ptr<GraphMaterialTextureData> tex;
             if(fileImage2SamplerUids.count(image_path) == 0)
             {
                tex = std::make_shared<GraphMaterialTextureData>();
                tex->handle = loadTexture(image_path,BGFX_TEXTURE_NONE|BGFX_SAMPLER_NONE,0,&tex->textureInfo);
                fileImage2SamplerUids[image_path] = tex;
             }else{
                tex = fileImage2SamplerUids.at(image_path);
                if (!bgfx::isValid(tex->handle)){
                   tex->handle = loadTexture(image_path,BGFX_TEXTURE_NONE|BGFX_SAMPLER_NONE,0,&tex->textureInfo);
                }
             }
            if(textureInfo != nullptr){
                *textureInfo = &tex->textureInfo;
            }
            return tex->handle;
        }
        
  
    };
}
