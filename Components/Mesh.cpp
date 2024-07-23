#include "Components/Mesh.h"

#include "glad/gl.h"

Mesh::Mesh(std::vector<glm::vec3> positions, std::vector<glm::vec2> textureCoords, std::vector<unsigned int> indices) {
    Vertices.reserve(positions.size());
    TextureCoords.reserve(textureCoords.size());
    Indices.reserve(indices.size());

    Vertices = std::move(positions);
    TextureCoords = std::move(textureCoords);
    Indices = std::move(indices);
}

Mesh::~Mesh() {
    glDeleteVertexArrays(1, &Vao);
    glDeleteBuffers(1, &Vbo);
    glDeleteBuffers(1, &Ebo);
}
