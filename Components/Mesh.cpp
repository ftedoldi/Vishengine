#include "Components/Mesh.h"

#include "glad/gl.h"

Mesh::Mesh(const std::vector<Vertex>& vertices, const std::vector<unsigned int>& indices) {
    Vertices.reserve(vertices.size());
    Indices.reserve(indices.size());

    Vertices = vertices;
    Indices = indices;
}

Mesh::~Mesh() {
    glDeleteVertexArrays(1, &Vao);
    glDeleteBuffers(1, &Vbo);
    glDeleteBuffers(1, &Ebo);
}
