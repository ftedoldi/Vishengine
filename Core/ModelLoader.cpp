#include "ModelLoader.h"

#include "Components/BoundingBox.h"
#include "Components/BoundingSphere.h"
#include "Components/MeshNode.h"
#include "Components/Name.h"
#include "Components/Relationship.h"
#include "Components/Transforms/RelativeTransform.h"
#include "Components/Transforms/TransformDirtyFlag.h"
#include "Components/Transforms/WorldTransform.h"
#include "DataStructures/Box.h"
#include "Material/Texture.h"

#include "assimp/postprocess.h"

#include <iostream>
#include <limits>
#include <ranges>
#include <unordered_map>

namespace {

Transform DecomposeMatrixIntoTransform(const aiMatrix4x4& transformMatrix) {
    aiVector3D aiScaling{};
    aiVector3D aiTranslation{};
    aiQuaternion aiRotation{};
    transformMatrix.Decompose(aiScaling, aiRotation, aiTranslation);

    return Transform{{aiTranslation.x, aiTranslation.y, aiTranslation.z},
        {aiRotation.x, aiRotation.y, aiRotation.z, aiRotation.w},
        (aiScaling.x + aiScaling.y + aiScaling.z) / 3.f};
}

}

ModelLoader::ModelLoader(entt::registry& registry,
                         MeshController* meshController,
                         MaterialController* materialController) : _registry{registry}, _meshController(meshController), _materialController(materialController) {}

void ModelLoader::ImportModel(const std::string& modelPath) {
    const auto* const scene{_importer.ReadFile(modelPath,aiProcess_Triangulate | aiProcess_JoinIdenticalVertices | aiProcess_FlipUVs | aiProcess_OptimizeMeshes/* | aiProcess_OptimizeGraph*/)};
    assert(scene);

    _modelDirectory = modelPath.substr(0, modelPath.find_last_of('/'));

    _processNode(scene->mRootNode, scene, entt::null, aiMatrix4x4{});
    _importer.FreeScene();
}

void ModelLoader::_processNode(const aiNode* const node,
                               const aiScene* const scene,
                               entt::entity parentEntity,
                               const aiMatrix4x4& accumulatedTransform) {
    assert(_meshController);
    assert(_materialController);
    // Accumulate this node's local transform on top of all ancestor transforms.
    aiMatrix4x4 worldTransform{accumulatedTransform * node->mTransformation};

    if (node->mNumMeshes > 0) {
        // Create a node entity that holds the accumulated transform.
        // All mesh entities for this node become children of this node entity
        // with identity transforms, keeping the hierarchy correct even when
        // a single node contains multiple meshes.
        const auto nodeEntity{_createNodeEntity(parentEntity, worldTransform, node->mName.C_Str())};

        if (parentEntity != entt::null) {
            auto& [_, currentNumberOfChildren, children]{_registry.get<Relationship>(parentEntity)};
            children[currentNumberOfChildren++] = nodeEntity;
            assert(currentNumberOfChildren < Relationship::MAX_NUM_OF_CHILDREN);
        }

        // Pass 1: compute bounding box (min/max) and sphere center (average vertex)
        // Streaming over Assimp data - no large intermediate buffer needed.
        glm::vec3 bboxMin{std::numeric_limits<float>::max()};
        glm::vec3 bboxMax{std::numeric_limits<float>::lowest()};
        glm::vec3 sphereCenter{};
        uint32_t totalVertices{};
        for (uint32_t i{0}; i < node->mNumMeshes; ++i) {
            const auto* const mesh{scene->mMeshes[node->mMeshes[i]]};
            for (uint32_t v{0}; v < mesh->mNumVertices; ++v) {
                const glm::vec3 pos{mesh->mVertices[v].x, mesh->mVertices[v].y, mesh->mVertices[v].z};
                bboxMin = glm::min(bboxMin, pos);
                bboxMax = glm::max(bboxMax, pos);
                sphereCenter += pos;
            }
            totalVertices += mesh->mNumVertices;
        }
        sphereCenter /= static_cast<float>(totalVertices);

        auto& meshNodeChildren{_registry.get<MeshNode>(nodeEntity).Meshes};
        for (uint32_t i{0}; i < node->mNumMeshes; ++i) {
            const auto aiMeshIndex{node->mMeshes[i]};
            const auto* const aiMesh{scene->mMeshes[aiMeshIndex]};
            const auto mesh{_processMesh(aiMesh, aiMeshIndex, scene)};
            meshNodeChildren.push_back(mesh);
        }

        // Pass 2: compute sphere radius (requires center from pass 1)
        float radiusSquared{};
        for (uint32_t i{0}; i < node->mNumMeshes; ++i) {
            const auto* const mesh{scene->mMeshes[node->mMeshes[i]]};
            for (uint32_t v{0}; v < mesh->mNumVertices; ++v) {
                const glm::vec3 pos{mesh->mVertices[v].x, mesh->mVertices[v].y, mesh->mVertices[v].z};
                const auto delta{pos - sphereCenter};
                radiusSquared = std::max(radiusSquared, glm::dot(delta, delta));
            }
        }

        _registry.emplace<BoundingSphere>(nodeEntity, sphereCenter, glm::sqrt(radiusSquared));
        _registry.emplace<BoundingBox>(nodeEntity, Box{bboxMin, bboxMax});
        worldTransform = aiMatrix4x4{};
        parentEntity = nodeEntity;
    }

    for(uint32_t i{0}; i < node->mNumChildren; ++i) {
        _processNode(node->mChildren[i], scene, parentEntity, worldTransform);
    }
}

entt::entity ModelLoader::_createNodeEntity(const entt::entity parentEntity, const aiMatrix4x4& matrixTransform, const std::string& nodeName) const {
    const auto nodeEntity{_registry.create()};

    auto& name{_registry.emplace<Name>(nodeEntity).Value};
    name = nodeName;

    auto& relationship{_registry.emplace<Relationship>(nodeEntity)};
    relationship.Parent = parentEntity;

    auto& relativeTransform{_registry.emplace<RelativeTransform>(nodeEntity).Value};
    const auto transform{DecomposeMatrixIntoTransform(matrixTransform)};
    relativeTransform.Position = transform.Position;
    relativeTransform.Rotation = transform.Rotation;
    relativeTransform.Scale = transform.Scale;

    _registry.emplace<TransformDirtyFlag>(nodeEntity);
    _registry.emplace<WorldTransform>(nodeEntity);
    _registry.emplace<MeshNode>(nodeEntity);

    return nodeEntity;
}

entt::entity ModelLoader::_processMesh(const aiMesh* const aiMesh, const uint32_t assimpMeshIndex, const aiScene* const scene) {
    const auto meshEntity{_registry.create()};
    _registry.emplace<WorldTransform>(meshEntity);

    if (const auto it{_processedMeshes.find(assimpMeshIndex)}; it != _processedMeshes.end()) {
        _registry.emplace<Mesh>(meshEntity, it->second.MeshID);
        return meshEntity;
    }

    std::vector<glm::vec3> vertices{};
    std::vector<glm::vec2> textureCoords{};
    std::vector<uint32_t> indices{};
    std::vector<glm::vec3> normals{};

    vertices.reserve(aiMesh->mNumVertices);

    // Process vertices
    for(uint32_t i{0}; i < aiMesh->mNumVertices; ++i) {
        vertices.emplace_back(aiMesh->mVertices[i].x, aiMesh->mVertices[i].y, aiMesh->mVertices[i].z);

        if(aiMesh->HasNormals()) {
            normals.emplace_back(aiMesh->mNormals[i].x, aiMesh->mNormals[i].y, aiMesh->mNormals[i].z);
        } else {
            normals.emplace_back(0.f, 1.f, 0.f);
        }

        if(aiMesh->mTextureCoords[0]) {
            textureCoords.emplace_back(aiMesh->mTextureCoords[0][i].x, aiMesh->mTextureCoords[0][i].y);
        }
        else {
            textureCoords.emplace_back(0.f, 0.f);
        }
    }

    // Process indices
    for(uint32_t i{0}; i < aiMesh->mNumFaces; ++i) {
        const auto face{aiMesh->mFaces[i]};
        for(uint32_t j{0}; j < face.mNumIndices; ++j) {
            indices.push_back(face.mIndices[j]);
        }
    }

    auto mesh{_meshController->CreateMesh({
            std::move(vertices),
            std::move(indices),
            std::move(textureCoords),
            std::move(normals)})};

    _processedMeshes.emplace(assimpMeshIndex, mesh);
    _registry.emplace<Mesh>(meshEntity, mesh);

    _processMaterials(scene, aiMesh, mesh.MeshID);

    return meshEntity;
}

std::vector<std::shared_ptr<Texture>> ModelLoader::_loadMaterialTextures(const aiMaterial* const mat, const aiTextureType type) {
    std::vector<std::shared_ptr<Texture>> textures;

    for(unsigned i{0}; i < mat->GetTextureCount(type); ++i) {
        aiString str;
        mat->GetTexture(type, i, &str);

        auto texturePath{_modelDirectory + "/" + str.C_Str()};

        if (const auto it{_texturesByPath.find(texturePath)}; it != _texturesByPath.end()) {
            textures.push_back(it->second);
            continue;
        }

        auto texture{std::make_shared<Texture>()};
        texture->Create(texturePath);
        _texturesByPath.emplace(std::move(texturePath), texture);
        textures.push_back(std::move(texture));
    }
    return textures;
}

std::vector<std::shared_ptr<Texture>> ModelLoader::_loadTextures(const aiScene* const scene, const aiMaterial* const material, const aiTextureType type) {
    std::vector<std::shared_ptr<Texture>> textures{};

    aiString texture_file{};
    material->Get(AI_MATKEY_TEXTURE(type, 0), texture_file);

    if(const auto* const aiTexture{scene->GetEmbeddedTexture(texture_file.C_Str())}){
        // Embedded textures share the cache with file textures; prefix the key
        // so an embedded "foo" can't collide with an on-disk path called "foo".
        auto cacheKey{std::string{"embedded:"} + texture_file.C_Str()};
        if (const auto it{_texturesByPath.find(cacheKey)}; it != _texturesByPath.end()) {
            textures.push_back(it->second);
        } else {
            auto texture{std::make_shared<Texture>()};
            texture->CreateEmbeddedTexture(aiTexture);
            _texturesByPath.emplace(std::move(cacheKey), texture);
            textures.push_back(std::move(texture));
        }
    }
    else {
        textures = _loadMaterialTextures(material, type);
    }

    return textures;
}

void ModelLoader::_processMaterials(const aiScene* const scene, const aiMesh* const mesh, const uint32_t meshID) {
    std::vector<std::shared_ptr<Texture>> texturesDiffuse{};
    std::vector<std::shared_ptr<Texture>> texturesSpecular{};
    glm::vec4 diffuseColor{};
    glm::vec3 specularColor{};

    const auto * const aiMaterial{scene->mMaterials[mesh->mMaterialIndex]};
    if(aiMaterial->GetTextureCount(aiTextureType_DIFFUSE) > 0) [[likely]]{
        texturesDiffuse = _loadTextures(scene, aiMaterial, aiTextureType_DIFFUSE);
    } else [[unlikely]] {
        aiColor4D diffuseColor4D{};
        if(aiMaterial->Get(AI_MATKEY_COLOR_DIFFUSE, diffuseColor4D) == AI_SUCCESS) {
            // This part of code is never reached thanks to a bug in Assimp: https://github.com/assimp/assimp/issues/6179?issue=assimp%7Cassimp%7C5543
            diffuseColor = {diffuseColor4D.r, diffuseColor4D.g, diffuseColor4D.b, diffuseColor4D.a};
        } else {
            // No color? You get a full black model :)
            diffuseColor = {0, 0, 0, 1};
            std::cout << "No color!" << "\n";
        }
    }

    if(aiMaterial->GetTextureCount(aiTextureType_SPECULAR) > 0) [[likely]] {
        texturesSpecular = _loadTextures(scene, aiMaterial, aiTextureType_SPECULAR);
    } else [[unlikely]] {
        aiColor3D specular{};

        if(aiMaterial->Get(AI_MATKEY_COLOR_SPECULAR, specular) == AI_SUCCESS) {
            specularColor = {specular.r, specular.g, specular.b};
        }
    }

    auto texturesNormal{_loadTextures(scene, aiMaterial, aiTextureType_NORMALS)};

    _materialController->AddMaterial(meshID, {
            std::move(texturesDiffuse),
            std::move(texturesSpecular),
            std::move(texturesNormal),
            diffuseColor,
            specularColor,
    });
}
