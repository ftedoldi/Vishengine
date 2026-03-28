#include "ModelLoader.h"

#include "Components/BoundingSphere.h"
#include "Components/InstancedMeshTag.h"
#include "Components/Relationship.h"
#include "Components/Transforms/RelativeTransform.h"
#include "Components/Transforms/TransformDirtyFlag.h"
#include "Components/Transforms/WorldTransform.h"
#include "Material/Texture.h"

#include "assimp/postprocess.h"

#include <iostream>

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
                         const std::shared_ptr<MeshController>& meshController,
                         const std::shared_ptr<MaterialController>& materialController) : _registry{registry}, _meshController(meshController), _materialController(materialController) {}

void ModelLoader::ImportModel(const std::string& modelPath) {
    const auto* const scene{_importer.ReadFile(modelPath,aiProcess_Triangulate | aiProcess_JoinIdenticalVertices | aiProcess_FlipUVs)};
    assert(scene);

    _modelDirectory = modelPath.substr(0, modelPath.find_last_of('/'));
    _processedMeshes.clear();

    _processNode(scene->mRootNode, scene, entt::null, aiMatrix4x4{});
}

void ModelLoader::_processNode(const aiNode* const node,
                               const aiScene* const scene,
                               entt::entity parentEntity,
                               const aiMatrix4x4& accumulatedTransform) {
    // Accumulate this node's local transform on top of all ancestor transforms.
    aiMatrix4x4 worldTransform{accumulatedTransform * node->mTransformation};

    if (node->mNumMeshes == 1) {
        const auto meshIndex{node->mMeshes[0]};
        auto* const mesh{scene->mMeshes[meshIndex]};
        parentEntity = _processMesh(mesh, scene, parentEntity, meshIndex);
        auto& relativeTransform{_registry.emplace<RelativeTransform>(parentEntity).Value};
        const auto transform{DecomposeMatrixIntoTransform(worldTransform)};
        relativeTransform.Position = transform.Position;
        relativeTransform.Rotation = transform.Rotation;
        relativeTransform.Scale = transform.Scale;
        worldTransform = aiMatrix4x4{};
    }

    if (node->mNumMeshes > 1) {
        // Create a node entity that holds the accumulated transform.
        // All mesh entities for this node become children of this node entity
        // with identity transforms, keeping the hierarchy correct even when
        // a single node contains multiple meshes.
        const auto nodeEntity{_createNodeEntity(parentEntity, worldTransform)};

        for(uint32_t i{0}; i < node->mNumMeshes; ++i) {
            const auto meshIndex{node->mMeshes[i]};
            const auto* const mesh{scene->mMeshes[meshIndex]};
            const auto entity{_processMesh(mesh, scene, nodeEntity, meshIndex)};
            _registry.emplace<RelativeTransform>(entity);
        }
        worldTransform = aiMatrix4x4{};
        parentEntity = nodeEntity;
    }

    for(uint32_t i{0}; i < node->mNumChildren; ++i) {
        _processNode(node->mChildren[i], scene, parentEntity, worldTransform);
    }
}


entt::entity ModelLoader::_createNodeEntity(const entt::entity parentEntity, const aiMatrix4x4& matrixTransform) const {
    const auto nodeEntity{_registry.create()};

    auto& relationship{_registry.emplace<Relationship>(nodeEntity)};
    relationship.parent = parentEntity;

    auto& relativeTransform{_registry.emplace<RelativeTransform>(nodeEntity).Value};
    const auto transform{DecomposeMatrixIntoTransform(matrixTransform)};
    relativeTransform.Position = transform.Position;
    relativeTransform.Rotation = transform.Rotation;
    relativeTransform.Scale = transform.Scale;

    return nodeEntity;
}

entt::entity ModelLoader::_processMesh(const aiMesh* const aiMesh, const aiScene* const scene, const entt::entity parentEntity, const uint32_t assimpMeshIndex) {
    const auto meshEntity{_registry.create()};

    auto& relationship{_registry.emplace<Relationship>(meshEntity)};
    relationship.parent = parentEntity;

    _registry.emplace<WorldTransform>(meshEntity);
    _registry.emplace<TransformDirtyFlag>(meshEntity);

    if (const auto it{_processedMeshes.find(assimpMeshIndex)}; it != _processedMeshes.end()) {
        _registry.emplace<InstancedMesh>(meshEntity, it->second.meshID);
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

    const auto mesh{_meshController->CreateMesh({
            std::move(vertices),
            std::move(indices),
            std::move(textureCoords),
            std::move(normals)})};

    _processedMeshes.emplace(assimpMeshIndex, mesh);
    _registry.emplace<Mesh>(meshEntity, mesh);

    _processMaterials(scene, aiMesh, mesh.meshID);
    _generateBoundingSphere(meshEntity, mesh.meshID);

    return meshEntity;
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

void ModelLoader::_processMaterials(const aiScene* const scene, const aiMesh* const mesh, const uint32_t meshID) {
    std::vector<Texture> texturesDiffuse{};
    std::vector<Texture> texturesSpecular{};
    glm::vec4 diffuseColor{};
    glm::vec3 specularColor{};

    auto* const aiMaterial{scene->mMaterials[mesh->mMaterialIndex]};
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

void ModelLoader::_generateBoundingSphere(const entt::entity meshEntity, const uint32_t meshID) const {
    const auto& rawMeshData{_meshController->GetRawMeshData(meshID)};
    const auto& vertices{rawMeshData.Vertices};

    if (vertices.empty()) {
        _registry.emplace<BoundingSphere>(meshEntity, BoundingSphere{{0.f, 0.f, 0.f}, 0.f});
        return;
    }

    glm::vec3 center{};
    for (const auto& vertex : vertices) {
        center += vertex;
    }
    center /= static_cast<float>(vertices.size());

    float radiusSquared{};
    for (const auto& vertex : vertices) {
        const auto delta{vertex - center};
        radiusSquared = std::max(radiusSquared, glm::dot(delta, delta));
    }

    _registry.emplace<BoundingSphere>(meshEntity, center, glm::sqrt(radiusSquared));
}
