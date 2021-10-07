/*
* Copyright 2021-2021 Zhouwei. All rights reserved.
* License: https://github.com/zwluoqi/mobile-visual-shader-editor#license-bsd-2-clause
*/

#pragma once

#include <vector>
#include <memory>
#include <map>


#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>


#include "Mesh.h"
#include "MeshFilter.h"

namespace userengine {
    class MeshUtil{
    public:
        std::vector<std::shared_ptr<userengine::MeshFilter>> load_mesh_filters(std::string& path);
    private:
        std::vector<std::shared_ptr<userengine::MeshFilter>> process_ai_nodes(aiNode* node, const aiScene* scene,std::string& directory);
        std::shared_ptr<userengine::Mesh> process_ai_meshs(aiMesh* mesh, const aiScene* scene,std::string& directory);
        std::vector<userengine::Texture> process_ai_material_tex(aiMaterial* mat, aiTextureType type, std::string typeName,std::string& directory);
        
        Assimp::Importer importer;
    };
}
