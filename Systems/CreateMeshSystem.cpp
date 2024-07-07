#include "CreateMeshSystem.h"

#include "Components/Transform.h"
#include "Components/Mesh.h"
#include "Components/TextureList.h"

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>

#include "glad/gl.h"

namespace CreateMeshSystemHelpers{
void setupMesh(Mesh& mesh) {

    // Modern openGL: see https://github.com/fendevel/Guide-to-Modern-OpenGL-Functions for reference

    glCreateBuffers(1, &mesh.Vbo);
    // Loads the vertices in the VBO
    glNamedBufferData(mesh.Vbo, sizeof(Vertex) * mesh.Vertices.size(), &mesh.Vertices[0], GL_STATIC_DRAW);

    glCreateBuffers(1, &mesh.Ebo);
    // Loads the indices in the VBO
    glNamedBufferData(mesh.Ebo, sizeof(unsigned int) * mesh.Indices.size(), &mesh.Indices[0], GL_STATIC_DRAW);

    glCreateVertexArrays(1, &mesh.Vao);
    // Lets vao know about the stride size for the vertices in the VBO
    glVertexArrayVertexBuffer(mesh.Vao, 0, mesh.Vbo, 0, sizeof(Vertex));

    // Bind the EBO to the VAO
    glVertexArrayElementBuffer(mesh.Vao, mesh.Ebo);

    glEnableVertexArrayAttrib(mesh.Vao, 0);
    glEnableVertexArrayAttrib(mesh.Vao, 1);

    glVertexArrayAttribFormat(mesh.Vao, 0, 3, GL_FLOAT, GL_FALSE, offsetof(Vertex, Position));
    glVertexArrayAttribFormat(mesh.Vao, 1, 2, GL_FLOAT, GL_FALSE, offsetof(Vertex, TextureCoord));

    glVertexArrayAttribBinding(mesh.Vao, 0, 0);
    glVertexArrayAttribBinding(mesh.Vao, 1, 0);
}
}


CreateMeshSystem::CreateMeshSystem(entt::registry& registry) : _registry{registry} {

}

void CreateMeshSystem::CreateMesh() {
    auto entity{_registry.create()};

    _registry.emplace<Transform>(entity, glm::vec3{0, 0, -6}, 1.f, glm::quat{0, 0, 0, 1});

    std::vector<Vertex> vertices;

    std::vector<unsigned int> indices{0, 1, 3, 1, 2, 3};

    Vertex vertex1{glm::vec3{0.5f,  0.5f, 0.0f}, glm::vec2{1.0f, 1.0f}};
    vertices.emplace_back(vertex1);

    Vertex vertex2{glm::vec3{0.5f, -0.5f, 0.0f}, glm::vec2{1.0f, 0.0f}};
    vertices.emplace_back(vertex2);

    Vertex vertex3{glm::vec3{-0.5f, -0.5f, 0.0f}, glm::vec2{0.0f, 0.0f}};
    vertices.emplace_back(vertex3);

    Vertex vertex4{glm::vec3{-0.5f,  0.5f, 0.0f}, glm::vec2{0.0f, 1.0f}};
    vertices.emplace_back(vertex4);

    auto& mesh{_registry.emplace<Mesh>(entity, vertices, indices)};
    CreateMeshSystemHelpers::setupMesh(mesh);

    auto& textureList{_registry.emplace<TextureList>(entity)};

    Texture texture1{};
    Texture texture2{};
    texture1.createTexture("../../Assets/container.jpg");
    texture2.createTexture("../../Assets/awesomeface.jpg");

    textureList.Textures.emplace_back(texture1);
    textureList.Textures.emplace_back(texture2);
}
