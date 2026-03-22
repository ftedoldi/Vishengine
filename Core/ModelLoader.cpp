#include "ModelLoader.h"

#include "Components/InstancedMesh.h"
#include "Components/Position.h"
#include "Components/Relationship.h"
#include "Components/Rotation.h"
#include "Components/Scale.h"

#include "Material/Texture.h"

#include "Components/WorldTransform.h"

#include "assimp/postprocess.h"

#include <iostream>

ModelLoader::ModelLoader(entt::registry& registry,
                         MeshController& meshController,
                         MaterialController& materialController) : _registry{registry},
                                                                   _meshController(meshController),
                                                                   _materialController(materialController) {}

std::optional<entt::entity> ModelLoader::ImportModel(const std::string& modelPath) {
    const aiScene* const scene{_importer.ReadFile(modelPath,aiProcess_Triangulate |
                                                                        aiProcess_JoinIdenticalVertices |
                                                                        aiProcess_FlipUVs)};
    if(!scene) {
        std::cout << "Error while loading a model" << std::endl;
        return std::nullopt;
    }

    _modelDirectory = modelPath.substr(0, modelPath.find_last_of('/'));
    _processedMeshes.clear();

    entt::entity rootEntity{_registry.create()};
    auto& relationship{_registry.emplace<Relationship>(rootEntity)};
    relationship.parent = entt::null;

    _registry.emplace<Position>(rootEntity, glm::vec3(0.0f));
    _registry.emplace<Rotation>(rootEntity, glm::quat(0.f, 0.f, 0.f, 1.f));
    _registry.emplace<Scale>(rootEntity, 1.);

    _processNode(scene->mRootNode, scene, rootEntity);

    return rootEntity;
}

void ModelLoader::_processNode(const aiNode* const node,
                               const aiScene* const scene,
                               const entt::entity parentEntity) {
    // 1. Always create an entity for the current Node (even if it's empty)
    const auto nodeEntity{_registry.create()};

    auto& relationship{_registry.emplace<Relationship>(nodeEntity)};
    relationship.parent = parentEntity;

    // 2. Extract ONLY the local transform of this specific node
    aiVector3D aiScaling{};
    aiVector3D aiTranslation{};
    aiQuaternion aiRotation{};
    node->mTransformation.Decompose(aiScaling, aiRotation, aiTranslation);

    const glm::vec3 translation{aiTranslation.x, aiTranslation.y, aiTranslation.z};
    const glm::quat rotation{aiRotation.x, aiRotation.y, aiRotation.z, aiRotation.w};
    const float uniformScale{(aiScaling.x + aiScaling.y + aiScaling.z) / 3.f};

    // 3. Set local Transform components
    _registry.emplace<Position>(nodeEntity, translation);
    _registry.emplace<Rotation>(nodeEntity, rotation);
    _registry.emplace<Scale>(nodeEntity, uniformScale);

    // 4. Process meshes as children attached to this nodeEntity
    for(uint32_t i{0}; i < node->mNumMeshes; ++i) {
        const auto meshIndex{node->mMeshes[i]};
        auto* const mesh{scene->mMeshes[meshIndex]};
        _processMesh(mesh, scene, nodeEntity, meshIndex);
    }

    // 5. Recursively process child nodes
    for(uint32_t i{0}; i < node->mNumChildren; ++i) {
        auto* child{node->mChildren[i]};
        // Pass nodeEntity as the new parent for the next depth level
        _processNode(child, scene, nodeEntity);
    }
}

void ModelLoader::_processMesh(aiMesh* const aiMesh, const aiScene* const scene, entt::entity parentEntity, const uint32_t assimpMeshIndex) {
    entt::entity meshEntity{_registry.create()};

    auto& relationship{_registry.emplace<Relationship>(meshEntity)};
    relationship.parent = parentEntity;

    _registry.emplace<Position>(meshEntity, glm::vec3{0.f, 0.f, 0.f});
    _registry.emplace<Rotation>(meshEntity, glm::quat{0.f, 0.f, 0.f, 1.f});
    _registry.emplace<Scale>(meshEntity, 1.f);
    _registry.emplace<WorldTransform>(meshEntity);

    if (const auto it{_processedMeshes.find(assimpMeshIndex)}; it != _processedMeshes.end()) {
        _registry.emplace<InstancedMesh>(meshEntity, it->second.meshID);
        return;
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

    const auto mesh{_meshController.CreateMesh({
            std::move(vertices),
            std::move(indices),
            std::move(textureCoords),
            std::move(normals)})};

    _processedMeshes.emplace(assimpMeshIndex, mesh);
    _registry.emplace<Mesh>(meshEntity, mesh);

    std::vector<Texture> texturesDiffuse{};
    std::vector<Texture> texturesSpecular{};
    glm::vec4 diffuseColor{};
    glm::vec3 specularColor{};

    auto* const aiMaterial{scene->mMaterials[aiMesh->mMaterialIndex]};
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

    const auto material{_materialController.AddMaterial({
            std::move(texturesDiffuse),
            std::move(texturesSpecular),
            std::move(texturesNormal),
            diffuseColor,
            specularColor,
    })};
    _registry.emplace<Material>(meshEntity, material);
}

std::vector<Texture> ModelLoader::_loadMaterialTextures(const aiMaterial* const mat, const aiTextureType type) {
    std::vector<Texture> textures;

    for(unsigned i{0}; i < mat->GetTextureCount(type); ++i) {
        aiString str;
        mat->GetTexture(type, i, &str);

        auto texturePath{_modelDirectory + "/" + str.C_Str()};

        bool textureAlreadyLoaded{false};
        for(const auto& texture : _loadedTextures) {
            if(texturePath == texture) {
                textureAlreadyLoaded = true;
            }
        }

        if(!textureAlreadyLoaded) {
            Texture texture;
            texture.CreateTexture(texturePath);
            textures.emplace_back(texture);

            _loadedTextures.emplace_back(std::move(texturePath));
        }
    }
    return textures;
}

std::vector<Texture> ModelLoader::_loadTextures(const aiScene* const scene, aiMaterial* const material, const aiTextureType type) {
    std::vector<Texture> textures{};

    aiString texture_file{};
    material->Get(AI_MATKEY_TEXTURE(type, 0), texture_file);

    if(const auto* aiTexture{scene->GetEmbeddedTexture(texture_file.C_Str())}){
        Texture texture{};
        texture.CreateEmbeddedTexture(aiTexture);
        textures.emplace_back(texture);
    }
    else {
        textures = _loadMaterialTextures(material, type);
    }

    return textures;
}
