#include "LoadModelSystem.h"

#include "Texture/Texture.h"

#include <iostream>

namespace LoadModelHelpers {

void SetupMesh(Mesh& mesh) {
    // Modern openGL: see https://github.com/fendevel/Guide-to-Modern-OpenGL-Functions for reference
    glCreateBuffers(1, &mesh.Vbo);

    // Loads the vertices in the VBO
    const auto verticesSize{mesh.Vertices.size() * sizeof(glm::vec3)};
    const auto texCoordsSize{mesh.TextureCoords.size() * sizeof(glm::vec2)};

    const auto totalSize{verticesSize + texCoordsSize};

    glNamedBufferData(mesh.Vbo, totalSize, nullptr, GL_STATIC_DRAW);
    glNamedBufferSubData(mesh.Vbo, 0, verticesSize, &mesh.Vertices[0]);
    glNamedBufferSubData(mesh.Vbo, verticesSize, texCoordsSize, &mesh.TextureCoords[0]);

    glCreateBuffers(1, &mesh.Ebo);
    // Loads the indices in the VBO
    glNamedBufferData(mesh.Ebo, sizeof(unsigned int) * mesh.Indices.size(), &mesh.Indices[0], GL_STATIC_DRAW);

    glCreateVertexArrays(1, &mesh.Vao);
    // Lets vao know about the stride size for the vertices in the VBO
    glVertexArrayVertexBuffer(mesh.Vao, 0, mesh.Vbo, 0, sizeof(glm::vec3));
    glVertexArrayVertexBuffer(mesh.Vao, 1, mesh.Vbo, verticesSize, sizeof(glm::vec2));

    // Bind the EBO to the VAO
    glVertexArrayElementBuffer(mesh.Vao, mesh.Ebo);

    glEnableVertexArrayAttrib(mesh.Vao, 0);
    glEnableVertexArrayAttrib(mesh.Vao, 1);

    glVertexArrayAttribFormat(mesh.Vao, 0, 3, GL_FLOAT, GL_FALSE, 0);
    glVertexArrayAttribFormat(mesh.Vao, 1, 2, GL_FLOAT, GL_FALSE, 0);

    glVertexArrayAttribBinding(mesh.Vao, 0, 0);
    glVertexArrayAttribBinding(mesh.Vao, 1, 1);
}

}

LoadModelSystem::LoadModelSystem(entt::registry& registry) : _registry{registry}{

}

void LoadModelSystem::ImportModel(const std::string& modelPath) {
    const aiScene* const scene{_importer.ReadFile(modelPath,aiProcess_Triangulate)};
    if(!scene) {
        std::cout << "Error while loading a model" << std::endl;
        return;
    }

    auto meshEntity{_registry.create()};
    _mesh = &_registry.emplace<Mesh>(meshEntity);
    _transform = &_registry.emplace<Transform>(meshEntity, glm::vec3{0, 0, -6}, 1.f, glm::quat{0, 0, 0, 1});
    _textureList = &_registry.emplace<TextureList>(meshEntity);

    _modelDirectory = modelPath.substr(0, modelPath.find_last_of('/'));

    _processNode(scene->mRootNode, scene);

    LoadModelHelpers::SetupMesh(*_mesh);
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
    // Process vertices
    for(unsigned i{0}; i < aiMesh->mNumVertices; ++i) {
        auto currentAiMeshVertex{aiMesh->mVertices[i]};

        glm::vec3 vertex{currentAiMeshVertex.x, currentAiMeshVertex.y, currentAiMeshVertex.z};
        _mesh->Vertices.push_back(vertex);

        if(aiMesh->mTextureCoords[0]) {
            glm::vec2 vec{};
            vec.x = aiMesh->mTextureCoords[0][i].x;
            vec.y = aiMesh->mTextureCoords[0][i].y;
            _mesh->TextureCoords.push_back(vec);
        }
        else {
            _mesh->TextureCoords.emplace_back(0.f, 0.f);
        }
    }

    // Process indices
    for(unsigned i{0}; i < aiMesh->mNumFaces; ++i) {
        const auto face{aiMesh->mFaces[i]};
        for(unsigned j{0}; j < face.mNumIndices; ++j) {
            _mesh->Indices.push_back(face.mIndices[j]);
        }
    }

    auto* const material{scene->mMaterials[aiMesh->mMaterialIndex]};
    _loadMaterialTextures(material, aiTextureType_DIFFUSE);
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
