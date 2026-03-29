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
    ModelLoader(entt::registry& registry, const std::shared_ptr<MeshController>& meshController, const std::shared_ptr<MaterialController>& materialController);

    void ImportModel(const std::string& modelPath);

private:
    void _processNode(const aiNode* node, const aiScene* scene, entt::entity parentEntity, const aiMatrix4x4& accumulatedTransform);

    entt::entity _createNodeEntity(entt::entity parentEntity, const aiMatrix4x4& matrixTransform, const std::string& nodeName) const;

    Mesh _processMesh(const aiMesh* aiMesh, const aiScene* scene, entt::entity parentEntity, uint32_t assimpMeshIndex);

    std::vector<Texture> _loadMaterialTextures(const aiMaterial* mat, aiTextureType type);

    std::vector<Texture> _loadTextures(const aiScene* scene, const aiMaterial* material, aiTextureType type);

    void _processMaterials(const aiScene* scene, const aiMesh* mesh, uint32_t meshID);

    void _generateBoundingSphere(entt::entity meshNodeEntity, const std::vector<glm::vec3>& objectVertices) const;

    entt::registry& _registry;

    std::shared_ptr<MeshController> _meshController{};

    std::shared_ptr<MaterialController> _materialController{};

    Assimp::Importer _importer{};

    std::string _modelDirectory{};

    std::vector<std::string> _loadedTextures{};

    std::unordered_map<uint32_t, Mesh> _processedMeshes{};
};
