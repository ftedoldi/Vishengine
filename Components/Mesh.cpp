#include "Components/Mesh.h"

#include "glad/gl.h"

Mesh::Mesh(std::vector<glm::vec3> vertices,
           std::vector<glm::vec2> textureCoords,
           std::vector<unsigned int> indices,
           std::vector<glm::vec3> normals) : _vertices{std::move(vertices)},
                                             _textureCoords{std::move(textureCoords)},
                                             Indices{std::move(indices)},
                                             _normals{std::move(normals)} {

    // Modern openGL: see https://github.com/fendevel/Guide-to-Modern-OpenGL-Functions for reference
    glCreateBuffers(1, &_vbo);

    // Loads the vertices in the VBO
    const auto verticesSize{_vertices.size() * sizeof(glm::vec3)};
    const auto texCoordsSize{_textureCoords.size() * sizeof(glm::vec2)};
    const auto normalsSize{_normals.size() * sizeof(glm::vec3)};

    const auto totalSize{verticesSize + texCoordsSize + normalsSize};

    glNamedBufferData(_vbo, totalSize, nullptr, GL_STATIC_DRAW);
    glNamedBufferSubData(_vbo, 0, verticesSize, &_vertices[0]);
    glNamedBufferSubData(_vbo, verticesSize, texCoordsSize, &_textureCoords[0]);
    glNamedBufferSubData(_vbo, verticesSize + texCoordsSize, normalsSize, &_normals[0]);

    glCreateBuffers(1, &_ebo);
    // Loads the indices in the VBO
    glNamedBufferData(_ebo, sizeof(unsigned int) * Indices.size(), &Indices[0], GL_STATIC_DRAW);

    glCreateVertexArrays(1, &Vao);
    // Lets vao know about the stride size for the vertices in the VBO
    glVertexArrayVertexBuffer(Vao, 0, _vbo, 0, sizeof(glm::vec3));
    glVertexArrayVertexBuffer(Vao, 1, _vbo, verticesSize, sizeof(glm::vec2));
    glVertexArrayVertexBuffer(Vao, 2, _vbo, verticesSize + texCoordsSize, sizeof(glm::vec3));

    // Bind the EBO to the VAO
    glVertexArrayElementBuffer(Vao, _ebo);

    glEnableVertexArrayAttrib(Vao, 0);
    glEnableVertexArrayAttrib(Vao, 1);
    glEnableVertexArrayAttrib(Vao, 2);

    glVertexArrayAttribFormat(Vao, 0, 3, GL_FLOAT, GL_FALSE, 0);
    glVertexArrayAttribFormat(Vao, 1, 2, GL_FLOAT, GL_FALSE, 0);
    glVertexArrayAttribFormat(Vao, 2, 3, GL_FLOAT, GL_FALSE, 0);

    glVertexArrayAttribBinding(Vao, 0, 0);
    glVertexArrayAttribBinding(Vao, 1, 1);
    glVertexArrayAttribBinding(Vao, 2, 2);
}

Mesh::~Mesh() {
    glDeleteVertexArrays(1, &Vao);
    glDeleteBuffers(1, &_vbo);
    glDeleteBuffers(1, &_ebo);
}

void Mesh::SetColorDiffuse(const glm::vec4 colorDiffuse) {
    _colorDiffuse = colorDiffuse;
}

glm::vec4 Mesh::GetColorDiffuse() const {
    return _colorDiffuse;
}

void Mesh::SetColorSpecular(const glm::vec3 colorSpecular) {
    _colorSpecular = colorSpecular;
}

glm::vec3 Mesh::GetColorSpecular() const {
    return _colorSpecular;
}

void Mesh::SetHasTextureDiffuse(const bool hasTextureDiffuse) {
    _hasTextureDiffuse = hasTextureDiffuse;
}

bool Mesh::GetHasTextureDiffuse() const {
    return _hasTextureDiffuse;
}

void Mesh::SetHasTextureSpecular(const bool hasTextureSpecular) {
    _hasTextureSpecular = hasTextureSpecular;
}

bool Mesh::GetHasTextureSpecular() const {
    return _hasTextureSpecular;
}
