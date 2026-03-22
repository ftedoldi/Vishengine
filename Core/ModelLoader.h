#pragma once

#include "Components/Mesh.h"
#include "Mesh/MeshController.h"
#include "Material/MaterialController.h"

#include <entt/entt.hpp>
#include "assimp/Importer.hpp"
#include "assimp/scene.h"

#include <optional>
#include <string>
#include <unordered_map>

class ModelLoader {
public:
    ModelLoader(entt::registry& registry, MeshController& meshController, MaterialController& materialController);

    std::optional<entt::entity> ImportModel(const std::string& modelPath);

private:
    void _processNode(const aiNode* node, const aiScene* scene, entt::entity parentEntity);

    void _processMesh(aiMesh* aiMesh, const aiScene* scene, entt::entity parentEntity, uint32_t assimpMeshIndex);

    std::vector<Texture> _loadMaterialTextures(const aiMaterial* mat, aiTextureType type);

    std::vector<Texture> _loadTextures(const aiScene* scene, aiMaterial* mat, aiTextureType type);

    entt::registry& _registry;

    MeshController& _meshController;

    MaterialController& _materialController;

    Assimp::Importer _importer{};

    std::string _modelDirectory{};

    std::vector<std::string> _loadedTextures{};

    std::unordered_map<uint32_t, Mesh> _processedMeshes{};
};
