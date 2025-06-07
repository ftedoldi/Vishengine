#include "ModelLoader.h"

#include "Components/Position.h"
#include "Components/Rotation.h"
#include "Components/Scale.h"
#include "Components/Relationship.h"

#include "Material/Texture.h"

#include "stb_image.h"

#include <iostream>

ModelLoader::ModelLoader(entt::registry& registry,
                         MeshController& meshController,
                         MaterialController& materialController) : _registry{registry},
                                                                   _meshController(meshController),
                                                                   _materialController(materialController) {}

std::optional<entt::entity> ModelLoader::ImportModel(const std::string& modelPath) {
    const aiScene* const scene{_importer.ReadFile(modelPath,aiProcess_Triangulate |
                                                                        aiProcess_JoinIdenticalVertices |
                                                                        aiProcess_OptimizeMeshes |
                                                                        aiProcess_OptimizeGraph | aiProcess_FlipUVs)};
    if(!scene) {
        std::cout << "Error while loading a model" << std::endl;
        return std::nullopt;
    }

    _modelDirectory = modelPath.substr(0, modelPath.find_last_of('/'));

    entt::entity rootEntity{_registry.create()};
    auto& relationship{_registry.emplace<Relationship>(rootEntity)};
    relationship.parent = entt::null;

    _registry.emplace<Position>(rootEntity, glm::vec3{0., 0., 0.});
    _registry.emplace<Rotation>(rootEntity, glm::quat{0., 0., 0., 1.});
    _registry.emplace<Scale>(rootEntity, 1.f);

    _processNode(scene->mRootNode, scene, rootEntity, aiMatrix4x4{});

    return rootEntity;
}

void ModelLoader::_processNode(aiNode* const node, const aiScene* const scene, entt::entity parentEntity, const aiMatrix4x4& accTransform) {
    entt::entity currentEntity{};

    aiMatrix4x4 transform{};

    // Only create an entity if the node has meshes
    if(node->mNumMeshes > 0) {
        currentEntity = _registry.create();
        auto& relationship{_registry.emplace<Relationship>(currentEntity)};
        relationship.parent = parentEntity;  // Set the parent entity

        aiVector3f aiTranslation{};
        aiQuaternion aiRotation{};

        transform = node->mTransformation;

        transform.DecomposeNoScaling(aiRotation, aiTranslation);

        glm::vec3 translation{aiTranslation.x, aiTranslation.y, aiTranslation.z};
        glm::quat rotation{aiRotation.x, aiRotation.y, aiRotation.z, aiRotation.w};

        _registry.emplace<Position>(currentEntity, translation);
        _registry.emplace<Rotation>(currentEntity, rotation);
        _registry.emplace<Scale>(currentEntity, 1.f);

        // Process all meshes in this node
        for(uint32_t i{0}; i < node->mNumMeshes; ++i) {
            auto* const mesh{scene->mMeshes[node->mMeshes[i]]};
            _processMesh(mesh, scene, currentEntity);
        }
    } else {
        currentEntity = parentEntity;

        transform = node->mTransformation * accTransform;
    }

    // Recursively process each child node, passing the current entity as the parent
    for(uint32_t i{0}; i < node->mNumChildren; ++i) {
        auto* child{node->mChildren[i]};
        _processNode(child, scene, currentEntity, transform);
    }
}

void ModelLoader::_processMesh(aiMesh* const aiMesh, const aiScene* const scene, entt::entity parentEntity) {
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

    entt::entity meshEntity{_registry.create()};

    auto& relationship{_registry.emplace<Relationship>(meshEntity)};
    relationship.parent = parentEntity;  // Set the parent entity

    const auto mesh{_meshController.CreateMesh({
            std::move(vertices),
            std::move(indices),
            std::move(textureCoords),
            std::move(normals)})};

    _registry.emplace<Mesh>(meshEntity, mesh);

    std::vector<Texture> texturesDiffuse{};
    std::vector<Texture> texturesSpecular{};
    glm::vec4 diffuseColor{};
    glm::vec3 specularColor{};

    auto* const aiMaterial{scene->mMaterials[aiMesh->mMaterialIndex]};
    if(aiMaterial->GetTextureCount(aiTextureType_DIFFUSE) > 0) [[likely]]{
        texturesDiffuse = _loadTextures(scene, aiMaterial, aiTextureType_DIFFUSE);
    } else [[unlikely]] {
        aiColor4D diffuseColor4D{};;
        if(aiMaterial->Get(AI_MATKEY_COLOR_DIFFUSE, diffuseColor) == AI_SUCCESS) {
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

std::vector<Texture> ModelLoader::_loadMaterialTextures(aiMaterial* const mat, const aiTextureType type) {
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
