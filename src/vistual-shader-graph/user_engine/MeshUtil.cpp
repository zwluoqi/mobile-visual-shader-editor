/*
* Copyright 2021-2021 Zhouwei. All rights reserved.
* License: https://github.com/zwluoqi/mobile-visual-shader-editor#license-bsd-2-clause
*/

#include <vector>
#include <memory>
#include <map>
#include "Mesh.h"
#include "MeshFilter.h"
#include "MeshUtil.h"

namespace userengine {

    std::vector<std::shared_ptr<userengine::MeshFilter>> MeshUtil::load_mesh_filters(std::string& path){

        const aiScene* scene = importer.ReadFile(path, aiProcess_CalcTangentSpace | aiProcess_Triangulate | aiProcess_FlipUVs);
        if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
        {
            BX_ASSERT(false, "ERROR::ASSIMP::"+ importer.GetErrorString());
        }

        std::string directory = path.substr(0, path.find_last_of('/'));
        
        return process_ai_nodes(scene->mRootNode, scene,directory);
    }

    std::vector<std::shared_ptr<userengine::MeshFilter>> MeshUtil::process_ai_nodes(aiNode* node, const aiScene* scene,std::string& directory)
    {
        std::vector<std::shared_ptr<userengine::MeshFilter>> vectors;
        for (unsigned int m=0;m< node->mNumMeshes;m++)
        {
            aiMesh* aimesh = scene->mMeshes[node->mMeshes[m]];
            auto mesh = process_ai_meshs(aimesh, scene,directory);
            userengine::Material mat;
            vectors.push_back(std::make_shared<MeshFilter>(MeshFilter{mesh,mat}));
        }

        for (unsigned int j = 0; j < node->mNumChildren; j++)
        {
            auto childMeshs = process_ai_nodes(node->mChildren[j], scene,directory);
            for (auto & mesh : childMeshs) {
                vectors.push_back(mesh);
            }
        }
        return vectors;
    }


    std::shared_ptr<userengine::Mesh> MeshUtil::process_ai_meshs(aiMesh* mesh, const aiScene* scene,std::string& directory)
    {
        std::vector<userengine::Vertex> vertices;
        std::vector<uint16_t> indices;
        std::vector<userengine::Texture> textures;

        for (unsigned int i = 0; i < mesh->mNumVertices; i++)
        {
            userengine::Vertex vertex;
            // 处理顶点位置、法线和纹理坐标
            vertex.Position.X =  mesh->mVertices[i].x;
            vertex.Position.Y = mesh->mVertices[i].y;
            vertex.Position.Z = mesh->mVertices[i].z;


            vertex.Normal.X = mesh->mNormals[i].x;
            vertex.Normal.Y = mesh->mNormals[i].y;
            vertex.Normal.Z = mesh->mNormals[i].z;
            
            if(mesh->mTangents != nullptr){
                auto tanu = Vector3(mesh->mTangents[i].x,mesh->mTangents[i].y,mesh->mTangents[i].z);
                auto tanv = Vector3(mesh->mBitangents[i].x,mesh->mBitangents[i].y,mesh->mBitangents[i].z);
                auto normal  = vertex.Normal;
                auto ndt     = math::DotV3 (normal, tanu);
                auto nxt     = math::CrossV3(normal, tanu);
                auto tmp     = tanu - (normal*ndt);

                auto ntmp = math::NormalizeV3(tmp);
                vertex.Tangent.X = ntmp.X;
                vertex.Tangent.Y = ntmp.Y;
                vertex.Tangent.Z = ntmp.Z;
                vertex.Tangent.W = math::DotV3(nxt, tanv) < 0.0f ? -1.0f : 1.0f;
            }

            if (mesh->mTextureCoords[0]) // 网格是否有纹理坐标？
            {
                vertex.TextureCoordinate.X = mesh->mTextureCoords[0][i].x;
                vertex.TextureCoordinate.Y = mesh->mTextureCoords[0][i].y;
            }


            if (mesh->mColors[0]) {
                vertex.m_abgr = (((uint32_t)mesh->mColors[0][i].a) << 24)
                    + (((uint32_t)mesh->mColors[0][i].b) << 16)
                    + (((uint32_t)mesh->mColors[0][i].g) << 8)
                    + ((uint32_t)mesh->mColors[0][i].r)
                    ;
            }

            vertices.push_back(vertex);
        }

        // 处理索引
        //...
        
        // 处理材质
//        if (mesh->mMaterialIndex >= 0)
//        {
//            //...
//            aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
//
//            std::vector<userengine::Texture> diffuseMaps = process_ai_material_tex(material,
//                aiTextureType_DIFFUSE, "texture_diffuse",directory);
//            textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());
//            std::vector<userengine::Texture> specularMaps = process_ai_material_tex(material,
//                aiTextureType_SPECULAR, "texture_specular",directory);
//            textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());
//            std::vector<userengine::Texture> normalMaps = process_ai_material_tex(material,
//                aiTextureType_NORMALS, "texture_normal",directory);
//            textures.insert(textures.end(), normalMaps.begin(), normalMaps.end());
//            std::vector<userengine::Texture> emissiveMaps = process_ai_material_tex(material,
//                aiTextureType_EMISSIVE, "texture_emissive",directory);
//            textures.insert(textures.end(), emissiveMaps.begin(), emissiveMaps.end());
//        }


        for (unsigned int i = 0; i < mesh->mNumFaces; i++)
        {
            aiFace face = mesh->mFaces[i];
            for (unsigned int j = 0; j < face.mNumIndices; j++)
                indices.push_back(face.mIndices[j]);
        }

        return std::make_shared<userengine::Mesh>(Mesh{vertices, indices, textures});
    }


    std::vector<userengine::Texture> MeshUtil::process_ai_material_tex(aiMaterial* mat, aiTextureType type, std::string typeName,std::string& directory)
    {
        std::vector<userengine::Texture> textures;

        for (unsigned int i = 0; i < mat->GetTextureCount(type); i++)
        {
            aiString str;
            mat->GetTexture(type, i, &str);
            userengine::Texture texture;
            std::string texPath = str.C_Str();
            int lastPos = (int)texPath.find_last_of('/');
            if (lastPos < 0) {
                lastPos = (int)texPath.find_last_of('\\');

            }
            std::string texName;
            if (lastPos < 0) {
                texName = (texPath);
            }
            else {
                texName = (texPath).substr(lastPos);
            }
            
            texName = directory + "/" + texName;
            texture.m_textureColor = loadTexture(texName.c_str());
            texture.s_texColor = bgfx::createUniform(typeName.c_str(), bgfx::UniformType::Sampler);
            texture.type = typeName;
            texture.path = texName;
            textures.push_back(texture);
        }
        return textures;
    }
}
