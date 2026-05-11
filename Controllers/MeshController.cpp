#include "MeshController.h"

#include "Components/MeshNode.h"
#include "Components/Relationship.h"
#include "Events/GameEvents.h"
#include "glad/gl.h"
#include "glm/vec4.hpp"

MeshController::MeshController(entt::registry& registry) : _registry{registry} {
    registry.on_destroy<MeshNode>().connect<&MeshController::_onMeshBeginDeletion>(this);
}

Mesh MeshController::CreateMesh(RawMeshData&& rawMeshData) {
    MeshData meshData{};
    meshData.RawMeshData = std::move(rawMeshData);
    
    auto& vbo{meshData.MeshGpuData.Vbo};
    auto& ebo{meshData.MeshGpuData.Ebo};
    auto& vao{meshData.MeshGpuData.Vao};

    const auto& vertices{meshData.RawMeshData.Vertices};
    const auto& textureCoords{meshData.RawMeshData.TextureCoords};
    const auto& indices{meshData.RawMeshData.Indices};
    const auto& normals{meshData.RawMeshData.Normals};

    assert(!vertices.empty() && !indices.empty());
    assert(textureCoords.size() == vertices.size());
    assert(normals.size() == vertices.size());
    
    // Modern openGL: see https://github.com/fendevel/Guide-to-Modern-OpenGL-Functions for reference
    glCreateBuffers(1, &vbo);
    
    // Loads the vertices in the VBO
    const auto verticesSize{vertices.size() * sizeof(glm::vec3)};
    const auto texCoordsSize{textureCoords.size() * sizeof(glm::vec2)};
    const auto normalsSize{normals.size() * sizeof(glm::vec3)};

    const auto totalSize{verticesSize + texCoordsSize + normalsSize};

    glNamedBufferData(vbo, totalSize, nullptr, GL_STATIC_DRAW);
    glNamedBufferSubData(vbo, 0, verticesSize, &vertices[0]);
    glNamedBufferSubData(vbo, verticesSize, texCoordsSize, &textureCoords[0]);
    glNamedBufferSubData(vbo, verticesSize + texCoordsSize, normalsSize, &normals[0]);

    glCreateBuffers(1, &ebo);
    // Loads the indices in the VBO
    glNamedBufferData(ebo, sizeof(unsigned int) * indices.size(), &indices[0], GL_STATIC_DRAW);

    glCreateVertexArrays(1, &vao);
    // Lets vao know about the stride size for the vertices in the VBO
    glVertexArrayVertexBuffer(vao, 0, vbo, 0, sizeof(glm::vec3));
    glVertexArrayVertexBuffer(vao, 1, vbo, verticesSize, sizeof(glm::vec2));
    glVertexArrayVertexBuffer(vao, 2, vbo, verticesSize + texCoordsSize, sizeof(glm::vec3));

    // Bind the EBO to the VAO
    glVertexArrayElementBuffer(vao, ebo);

    glEnableVertexArrayAttrib(vao, 0);
    glEnableVertexArrayAttrib(vao, 1);
    glEnableVertexArrayAttrib(vao, 2);

    glVertexArrayAttribFormat(vao, 0, 3, GL_FLOAT, GL_FALSE, 0);
    glVertexArrayAttribFormat(vao, 1, 2, GL_FLOAT, GL_FALSE, 0);
    glVertexArrayAttribFormat(vao, 2, 3, GL_FLOAT, GL_FALSE, 0);

    glVertexArrayAttribBinding(vao, 0, 0);
    glVertexArrayAttribBinding(vao, 1, 1);
    glVertexArrayAttribBinding(vao, 2, 2);

    auto& instanceSsbo{meshData.MeshGpuData.InstanceSsbo};
    glCreateBuffers(1, &instanceSsbo);

    meshData.MeshGpuData.IndexCount = static_cast<uint32_t>(indices.size());
    meshData.RawMeshData = {};

    _meshIDToMeshData.emplace(_nextMeshId, std::move(meshData));
    return Mesh{_nextMeshId++};
}

const MeshGpuData& MeshController::GetMeshGpuData(const uint32_t meshID) const {
    assert(_meshIDToMeshData.contains(meshID));

    return _meshIDToMeshData.at(meshID).MeshGpuData;
}

const RawMeshData& MeshController::GetRawMeshData(const uint32_t meshID) const {
    assert(_meshIDToMeshData.contains(meshID));

    return _meshIDToMeshData.at(meshID).RawMeshData;
}

const MeshData& MeshController::GetMeshData(const uint32_t meshID) const {
    assert(_meshIDToMeshData.contains(meshID));

    return _meshIDToMeshData.at(meshID);
}

void MeshController::DeleteMesh(const uint32_t meshID) {
    assert(_meshIDToMeshData.contains(meshID));

    const auto& meshData{_meshIDToMeshData.at(meshID)};
    glDeleteVertexArrays(1, &meshData.MeshGpuData.Vao);
    glDeleteBuffers(1, &meshData.MeshGpuData.Vbo);
    glDeleteBuffers(1, &meshData.MeshGpuData.Ebo);
    glDeleteBuffers(1, &meshData.MeshGpuData.InstanceSsbo);

    _meshIDToMeshData.erase(meshID);
}

void MeshController::_onMeshBeginDeletion(const entt::entity meshNodeEntity) {
    const auto& meshNode{_registry.get<MeshNode>(meshNodeEntity)};
    for (const auto entity : meshNode.Meshes) {
        DeleteMesh(_registry.get<Mesh>(entity).MeshID);
    }
    // TODO: fix need to also delete the children
}