#pragma once

#include "Components/Mesh.h"
#include "Mesh/MeshController.h"
#include "Material/MaterialController.h"

#include "Libs/entt/src/entt/entt.hpp"
#include "assimp/Importer.hpp"
#include "assimp/postprocess.h"
#include "assimp/scene.h"
#include "glm/ext/quaternion_float.hpp"

#include <optional>
#include <string>

class ModelLoader {
public:
    ModelLoader(entt::registry& registry, MeshController& meshController, MaterialController& materialController);

    std::optional<entt::entity> ImportModel(const std::string& modelPath);

private:
    void _processNode(aiNode* node, const aiScene* scene, entt::entity parentEntity, const aiMatrix4x4& accTransform);

    void _processMesh(aiMesh* aiMesh, const aiScene* scene, entt::entity parentEntity);

    std::vector<Texture> _loadMaterialTextures(aiMaterial* mat, aiTextureType type);

    std::vector<Texture> _loadTextures(const aiScene* scene, aiMaterial* mat, aiTextureType type);

    entt::registry& _registry;

    MeshController& _meshController;

    MaterialController& _materialController;

    Assimp::Importer _importer{};

    std::string _modelDirectory{};

    std::vector<std::string> _loadedTextures{};
};