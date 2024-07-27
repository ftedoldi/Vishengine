#include "LoadModelSystem.h"

#include "Components/Position.h"
#include "Components/Rotation.h"
#include "Components/Scale.h"

#include "Texture/Texture.h"

#include <iostream>

LoadModelSystem::LoadModelSystem(entt::registry& registry) : _registry{registry} {

}

std::optional<entt::entity> LoadModelSystem::ImportModel(const std::string& modelPath) {
    const aiScene* const scene{_importer.ReadFile(modelPath,aiProcess_Triangulate |
                                                                        aiProcess_JoinIdenticalVertices |
                                                                        aiProcess_OptimizeMeshes |
                                                                        aiProcess_OptimizeGraph |
                                                                        aiProcess_FlipUVs)};
    if(!scene) {
        std::cout << "Error while loading a model" << std::endl;
        return std::nullopt;
    }

    auto meshEntity{_registry.create()};
    _meshObject = &_registry.emplace<MeshObject>(meshEntity);

    _registry.emplace<Position>(meshEntity, glm::vec3{0.f, 0.f, -6.f});
    _registry.emplace<Rotation>(meshEntity, glm::quat{0.f, 0.f, 0.f, 1.f});
    _registry.emplace<Scale>(meshEntity, 1.f);


    _modelDirectory = modelPath.substr(0, modelPath.find_last_of('/'));

    _processNode(scene->mRootNode, scene);

    return meshEntity;
}

void LoadModelSystem::_processNode(aiNode* const node, const aiScene* const scene) {
    for(unsigned i{0}; i < node->mNumMeshes; ++i) {
        auto* const mesh{scene->mMeshes[node->mMeshes[i]]};
        _processMesh(mesh, scene);
    }

    for(unsigned i{0}; i < node->mNumChildren; ++i) {
        _processNode(node->mChildren[i], scene);
    }
}

void LoadModelSystem::_processMesh(aiMesh* const aiMesh, const aiScene* const scene) {
    std::vector<glm::vec3> vertices;
    std::vector<glm::vec2> textureCoords;
    std::vector<unsigned int> indices;

    // Process vertices
    for(unsigned i{0}; i < aiMesh->mNumVertices; ++i) {
        auto currentAiMeshVertex{aiMesh->mVertices[i]};

        glm::vec3 vertex{currentAiMeshVertex.x, currentAiMeshVertex.y, currentAiMeshVertex.z};
        vertices.push_back(vertex);

        if(aiMesh->mTextureCoords[0]) {
            glm::vec2 vec{};
            vec.x = aiMesh->mTextureCoords[0][i].x;
            vec.y = aiMesh->mTextureCoords[0][i].y;
            textureCoords.push_back(vec);
        }
        else {
            textureCoords.emplace_back(0.f, 0.f);
        }
    }

    // Process indices
    for(unsigned i{0}; i < aiMesh->mNumFaces; ++i) {
        const auto face{aiMesh->mFaces[i]};
        for(unsigned j{0}; j < face.mNumIndices; ++j) {
            indices.push_back(face.mIndices[j]);
        }
    }

    auto mesh{std::make_shared<Mesh>(vertices, textureCoords, indices)};

    auto* const material{scene->mMaterials[aiMesh->mMaterialIndex]};

    if(material->GetTextureCount(aiTextureType_DIFFUSE) > 0) [[likely]] {
        mesh->TexturesDiffuse = _loadMaterialTextures(material, aiTextureType_DIFFUSE);
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

    _meshObject->Meshes.emplace_back(std::move(mesh));
}

std::vector<Texture> LoadModelSystem::_loadMaterialTextures(aiMaterial* mat, const aiTextureType type) {
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
