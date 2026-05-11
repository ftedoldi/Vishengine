#pragma once

#include "Controllers/MaterialController.h"
#include "Controllers/MeshController.h"
#include "Components/Mesh.h"

#include "assimp/Importer.hpp"
#include "assimp/scene.h"
#include <entt/entt.hpp>

#include <ankerl/unordered_dense.h>

#include <memory>
#include <string>
#include <vector>

class ModelLoader {
public:
    ModelLoader(entt::registry& registry, MeshController* meshController, MaterialController* materialController);

    void ImportModel(const std::string& modelPath);

private:
    void _processNode(const aiNode* node, const aiScene* scene, entt::entity parentEntity, const aiMatrix4x4& accumulatedTransform);

    entt::entity _createNodeEntity(entt::entity parentEntity, const aiMatrix4x4& matrixTransform, const std::string& nodeName) const;

    entt::entity _processMesh(const aiMesh* aiMesh, uint32_t assimpMeshIndex, const aiScene* scene);

    std::vector<std::shared_ptr<Texture>> _loadMaterialTextures(const aiMaterial* mat, aiTextureType type);

    std::vector<std::shared_ptr<Texture>> _loadTextures(const aiScene* scene, const aiMaterial* material, aiTextureType type);

    void _processMaterials(const aiScene* scene, const aiMesh* mesh, uint32_t meshID);

    entt::registry& _registry;

    MeshController* _meshController{};

    MaterialController* _materialController{};

    Assimp::Importer _importer{};

    std::string _modelDirectory{};

    ankerl::unordered_dense::map<std::string, std::shared_ptr<Texture>> _texturesByPath{};

    ankerl::unordered_dense::map<uint32_t, Mesh> _processedMeshes{};
};
