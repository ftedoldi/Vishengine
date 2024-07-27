#pragma once

#include "Components/Mesh.h"
#include "Components/Transform.h"
#include "Components/TextureList.h"
#include "Components/MeshObject.h"

#include <entt/entt.hpp>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <string>
#include <optional>

class LoadModelSystem {
public:
    explicit LoadModelSystem(entt::registry& registry);

    std::optional<entt::entity> ImportModel(const std::string& modelPath);

private:
    void _processNode(aiNode* node, const aiScene* scene);

    void _processMesh(aiMesh* aiMesh, const aiScene* scene);

    std::vector<Texture> _loadMaterialTextures(aiMaterial* mat, aiTextureType type);

    entt::registry& _registry;

    Transform* _transform{};

    Assimp::Importer _importer{};

    std::string _modelDirectory;

    std::vector<std::string> _loadedTextures{};

    MeshObject* _meshObject{};
};