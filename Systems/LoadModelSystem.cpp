#include "LoadModelSystem.h"

#include "Texture/Texture.h"

#include <iostream>

LoadModelSystem::LoadModelSystem(entt::registry& registry) : _registry{registry} {

}

void LoadModelSystem::ImportModel(const std::string& modelPath) {
    const aiScene* const scene{_importer.ReadFile(modelPath,aiProcess_Triangulate |
                                                                        aiProcess_JoinIdenticalVertices |
                                                                        aiProcess_OptimizeMeshes |
                                                                        aiProcess_OptimizeGraph |
                                                                        aiProcess_FlipUVs)};
    if(!scene) {
        std::cout << "Error while loading a model" << std::endl;
        return;
    }

    auto meshEntity{_registry.create()};
    _meshObject = &_registry.emplace<MeshObject>(meshEntity);

    _transform = &_registry.emplace<Transform>(meshEntity, glm::vec3{0, 0, -6}, 1.f, glm::quat{0, 0, 0, 1});
    _textureList = &_registry.emplace<TextureList>(meshEntity);

    _modelDirectory = modelPath.substr(0, modelPath.find_last_of('/'));

    _processNode(scene->mRootNode, scene);
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

    auto* const material{scene->mMaterials[aiMesh->mMaterialIndex]};
    _loadMaterialTextures(material, aiTextureType_DIFFUSE);

    _meshObject->Meshes.emplace_back(std::make_shared<Mesh>(vertices, textureCoords, indices));
}

void LoadModelSystem::_loadMaterialTextures(aiMaterial* mat, aiTextureType type) {
    //std::vector<Texture> textures;

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
            _textureList->TexturesDiffuse.emplace_back(texture);

            _loadedTextures.emplace_back(std::move(texturePath));
        }
    }
    //return textures;
}

void LoadModelSystem::_createTexture() {

}
