#include "MeshController.h"

#include "glad/gl.h"

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

    // TODO: change with a guid
    static MeshID meshID{0};
    _meshIDToMeshData.emplace(meshID, std::move(meshData));

    return Mesh{meshID++};
}

const MeshGpuData& MeshController::GetMeshGpuData(const MeshID meshID) const {
    assert(_meshIDToMeshData.contains(meshID));

    return _meshIDToMeshData.at(meshID).MeshGpuData;
}

const RawMeshData& MeshController::GetRawMeshData(const MeshID meshID) const {
    assert(_meshIDToMeshData.contains(meshID));

    return _meshIDToMeshData.at(meshID).RawMeshData;
}

const MeshData& MeshController::GetMeshData(const MeshID meshID) const {
    assert(_meshIDToMeshData.contains(meshID));

    return _meshIDToMeshData.at(meshID);
}

void MeshController::DeleteMesh(const MeshID& meshID) {
    assert(_meshIDToMeshData.contains(meshID));

    const auto& meshData{_meshIDToMeshData.at(meshID)};
    glDeleteVertexArrays(1, &meshData.MeshGpuData.Vao);
    glDeleteBuffers(1, &meshData.MeshGpuData.Vbo);
    glDeleteBuffers(1, &meshData.MeshGpuData.Ebo);

    _meshIDToMeshData.erase(meshID);
}

