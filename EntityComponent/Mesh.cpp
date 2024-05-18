#include "Mesh.h"

#include <glad/gl.h>

Mesh::Mesh(const std::vector<Vertex>& vertices, const std::vector<unsigned int>& indices){
    _vertices.reserve(vertices.size());
    _indices.reserve(indices.size());

    _vertices = vertices;
    _indices = indices;
}

Mesh::~Mesh() {
    glDeleteVertexArrays(1, &_vao);
    glDeleteBuffers(1, &_vbo);
    glDeleteBuffers(1, &_ebo);
}

void Mesh::Start() {
    _setupMesh();
}

void Mesh::Update() {
    glBindVertexArray(_vao);
    glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(_indices.size()), GL_UNSIGNED_INT, nullptr);
    glBindVertexArray(0);
}

void Mesh::_setupMesh() {
    // Modern openGL: see https://github.com/fendevel/Guide-to-Modern-OpenGL-Functions for reference

    glCreateBuffers(1, &_vbo);
    // Loads the vertices in the VBO
    glNamedBufferData(_vbo, sizeof(Vertex) * _vertices.size(), &_vertices[0], GL_STATIC_DRAW);

    glCreateBuffers(1, &_ebo);
    // Loads the indices in the VBO
    glNamedBufferData(_ebo, sizeof(unsigned int) * _indices.size(), &_indices[0], GL_STATIC_DRAW);

    glCreateVertexArrays(1, &_vao);
    // Lets vao know about the stride size for the vertices in the VBO
    glVertexArrayVertexBuffer(_vao, 0, _vbo, 0, sizeof(Vertex));

    // Bind the EBO to the VAO
    glVertexArrayElementBuffer(_vao, _ebo);

    glEnableVertexArrayAttrib(_vao, 0);
    glEnableVertexArrayAttrib(_vao, 1);

    glVertexArrayAttribFormat(_vao, 0, 3, GL_FLOAT, GL_FALSE, offsetof(Vertex, Position));
    glVertexArrayAttribFormat(_vao, 1, 2, GL_FLOAT, GL_FALSE, offsetof(Vertex, TextureCoord));

    glVertexArrayAttribBinding(_vao, 0, 0);
    glVertexArrayAttribBinding(_vao, 1, 0);
}
