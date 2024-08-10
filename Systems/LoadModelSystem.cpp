#include "LoadModelSystem.h"

#include "Components/Position.h"
#include "Components/Rotation.h"
#include "Components/Scale.h"
#include "Components/Relationship.h"

#include "Texture/Texture.h"

#include "stb_image.h"

#include <iostream>

LoadModelSystem::LoadModelSystem(entt::registry& registry) : _registry{registry} {

}

std::optional<entt::entity> LoadModelSystem::ImportModel(const std::string& modelPath) {
    const aiScene* const scene{_importer.ReadFile(modelPath,aiProcess_Triangulate |
                                                                        aiProcess_JoinIdenticalVertices |
                                                                        aiProcess_OptimizeMeshes |
                                                                        aiProcess_OptimizeGraph)};
    if(!scene) {
        std::cout << "Error while loading a model" << std::endl;
        return std::nullopt;
    }

    _modelDirectory = modelPath.substr(0, modelPath.find_last_of('/'));

    _processNode(scene->mRootNode, scene, entt::null, aiMatrix4x4{});

    return _rootEntity;
}

void LoadModelSystem::_processNode(aiNode* const node, const aiScene* const scene, entt::entity parentEntity, const aiMatrix4x4& accTransform) {
    entt::entity currentEntity;

    aiMatrix4x4 transform{};

    // Only create an entity if the node has meshes
    if(node->mNumMeshes > 0) {
        entt::entity newEntity{_registry.create()};

        aiVector3f aiTranslation{};
        aiQuaternion aiRotation{};

        const auto newTransform{node->mTransformation * transform};

        newTransform.DecomposeNoScaling(aiRotation, aiTranslation);

        glm::vec3 translation{aiTranslation.x, aiTranslation.y, aiTranslation.z};
        glm::quat rotation{aiRotation.x, aiRotation.y, aiRotation.z, aiRotation.w};

        // Add components to the current entity
        _registry.emplace<Position>(newEntity, translation);
        _registry.emplace<Rotation>(newEntity, rotation);
        _registry.emplace<Scale>(newEntity, 1.f);

        _registry.emplace<MeshObject>(newEntity);

        auto& relationship{_registry.emplace<Relationship>(newEntity)};
        relationship.parent = parentEntity;  // Set the parent entity

        // Process all meshes in this node
        for(uint32_t i{0}; i < node->mNumMeshes; ++i) {
            auto* const mesh{scene->mMeshes[node->mMeshes[i]]};
            _processMesh(mesh, scene, newEntity);
        }

        currentEntity = newEntity;
        transform = aiMatrix4x4{};

        if(_rootEntity == entt::null){
            _rootEntity = newEntity;
        }

    } else {
        currentEntity = parentEntity;

        transform = node->mTransformation * accTransform;
    }

    // Recursively process each child node, passing the current entity as the parent
    for(uint32_t i{0}; i < node->mNumChildren; ++i) {
        _processNode(node->mChildren[i], scene, currentEntity, transform);
    }
}

void LoadModelSystem::_processMesh(aiMesh* const aiMesh, const aiScene* const scene, const entt::entity meshEntity) {
    std::vector<glm::vec3> vertices;
    std::vector<glm::vec2> textureCoords;
    std::vector<unsigned int> indices;
    std::vector<glm::vec3> normals;

    vertices.reserve(aiMesh->mNumVertices);

    // Process vertices
    for(uint32_t i{0}; i < aiMesh->mNumVertices; ++i) {
        vertices.emplace_back(aiMesh->mVertices[i].x, aiMesh->mVertices[i].y, aiMesh->mVertices[i].z);

        if(aiMesh->HasNormals()) {
            normals.emplace_back(aiMesh->mNormals[i].x, aiMesh->mNormals[i].y, aiMesh->mNormals[i].z);
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

    auto& meshObject{_registry.get<MeshObject>(meshEntity)};

    auto mesh{std::make_shared<Mesh>(vertices, textureCoords, indices, normals)};

    auto* const material{scene->mMaterials[aiMesh->mMaterialIndex]};

    if(material->GetTextureCount(aiTextureType_DIFFUSE) > 0) [[likely]] {

        mesh->TexturesDiffuse = _loadEmbeddedTextures(scene, material, aiTextureType_DIFFUSE);
        //mesh.TexturesDiffuse = _loadMaterialTextures(material, aiTextureType_DIFFUSE);

        mesh->SetHasTextureDiffuse(true);
    } else [[unlikely]] {
        aiColor4D diffuseColor{};

        if(material->Get(AI_MATKEY_COLOR_DIFFUSE, diffuseColor) == AI_SUCCESS) {
            mesh->SetColorDiffuse({diffuseColor.r, diffuseColor.g, diffuseColor.b, diffuseColor.a});
        } else {
            // No color? You get a full black model :)
            mesh->SetColorDiffuse({0, 0, 0, 1});
            std::cout << "No color!" << "\n";
        }

        mesh->SetHasTextureDiffuse(false);
    }

    if(material->GetTextureCount(aiTextureType_SPECULAR) > 0) [[likely]] {
        mesh->TexturesSpecular = _loadMaterialTextures(material, aiTextureType_SPECULAR);
        mesh->SetHasTextureSpecular(true);
    } else [[unlikely]] {
        aiColor3D specular{};

        if(material->Get(AI_MATKEY_COLOR_SPECULAR, specular) == AI_SUCCESS) {
            mesh->SetColorSpecular({specular.r, specular.g, specular.b});
        }

        mesh->SetHasTextureSpecular(false);
    }

    mesh->TexturesNormal = _loadMaterialTextures(material, aiTextureType_NORMALS);

    meshObject.Meshes.emplace_back(mesh);
}

std::vector<Texture> LoadModelSystem::_loadMaterialTextures(aiMaterial* const mat, const aiTextureType type) {
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

std::vector<Texture> LoadModelSystem::_loadEmbeddedTextures(const aiScene* const scene, aiMaterial* const material, const aiTextureType type) {
    std::vector<Texture> textures;

    aiString texture_file;
    material->Get(AI_MATKEY_TEXTURE(type, 0), texture_file);

    if(const auto* aiTexture{scene->GetEmbeddedTexture(texture_file.C_Str())}){
        Texture texture;
        texture.CreateEmbeddedTexture(aiTexture);
        textures.emplace_back(texture);
    }

    return textures;
}
