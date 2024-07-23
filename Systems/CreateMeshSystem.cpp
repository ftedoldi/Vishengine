#include "CreateMeshSystem.h"

#include "Components/Transform.h"
#include "Components/Mesh.h"
#include "Components/TextureList.h"

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>

#include "glad/gl.h"

namespace LoadModelHelpers {
void setupMesh(Mesh& mesh) {
    // Modern openGL: see https://github.com/fendevel/Guide-to-Modern-OpenGL-Functions for reference
    glCreateBuffers(1, &mesh.Vbo);

    // Loads the vertices in the VBO
    const auto positionSize{mesh.Vertices.size() * sizeof(glm::vec3)};
    const auto texCoordsSize{mesh.TextureCoords.size() * sizeof(glm::vec2)};

    const auto totalSize{positionSize + texCoordsSize};

    glNamedBufferData(mesh.Vbo, totalSize, nullptr, GL_STATIC_DRAW);
    glNamedBufferSubData(mesh.Vbo, 0, positionSize, &mesh.Vertices[0]);
    glNamedBufferSubData(mesh.Vbo, positionSize, texCoordsSize, &mesh.TextureCoords[0]);

    glCreateBuffers(1, &mesh.Ebo);
    // Loads the indices in the VBO
    //glNamedBufferData(mesh.Ebo, sizeof(unsigned int) * mesh.Indices.size(), &mesh.Indices[0], GL_STATIC_DRAW);

    glCreateVertexArrays(1, &mesh.Vao);
    // Lets vao know about the stride size for the vertices in the VBO
    glVertexArrayVertexBuffer(mesh.Vao, 0, mesh.Vbo, 0, sizeof(glm::vec3));
    glVertexArrayVertexBuffer(mesh.Vao, 1, mesh.Vbo, positionSize, sizeof(glm::vec2));

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

CreateMeshSystem::CreateMeshSystem(entt::registry& registry) : _registry{registry} {

}

void CreateMeshSystem::CreateMesh() {
    auto entity{_registry.create()};

    _registry.emplace<Transform>(entity, glm::vec3{0, 0, -6}, 1.f, glm::quat{0, 0, 0, 1});

    /*std::vector<unsigned int> indices{0, 1, 3, 1, 2, 3};

    std::vector<glm::vec3> positions{};

    positions.emplace_back(0.5f,  0.5f, 0.0f);
    positions.emplace_back(0.5f, -0.5f, 0.0f);
    positions.emplace_back(-0.5f, -0.5f, 0.0f);
    positions.emplace_back(-0.5f,  0.5f, 0.0f);

    std::vector<glm::vec2> textureCoords{};

    textureCoords.emplace_back(1.0f, 1.0f);
    textureCoords.emplace_back(1.0f, 0.0f);
    textureCoords.emplace_back(0.0f, 0.0f);
    textureCoords.emplace_back(0.0f, 1.0f);*/

    std::vector<glm::vec3> positions;
    std::vector<glm::vec2> textureCoords;

    positions.emplace_back(-0.5f, -0.5f, -0.5f);
    textureCoords.emplace_back(0.0f, 0.0f);

    positions.emplace_back(0.5f, -0.5f, -0.5f);
    textureCoords.emplace_back(1.0f, 0.0f);

    positions.emplace_back(0.5f, 0.5f, -0.5f);
    textureCoords.emplace_back(1.0f, 1.0f);

    positions.emplace_back(0.5f, 0.5f, -0.5f);
    textureCoords.emplace_back(1.0f, 1.0f);

    positions.emplace_back(-0.5f, 0.5f, -0.5f);
    textureCoords.emplace_back(0.0f, 1.0f);

    positions.emplace_back(-0.5f, -0.5f, -0.5f);
    textureCoords.emplace_back(0.0f, 0.0f);

    positions.emplace_back(-0.5f, -0.5f, 0.5f);
    textureCoords.emplace_back(0.0f, 0.0f);

    positions.emplace_back(0.5f, -0.5f, 0.5f);
    textureCoords.emplace_back(1.0f, 0.0f);

    positions.emplace_back(0.5f, 0.5f, 0.5f);
    textureCoords.emplace_back(1.0f, 1.0f);

    positions.emplace_back(0.5f, 0.5f, 0.5f);
    textureCoords.emplace_back(1.0f, 1.0f);

    positions.emplace_back(-0.5f, 0.5f, 0.5f);
    textureCoords.emplace_back(0.0f, 1.0f);

    positions.emplace_back(-0.5f, -0.5f, 0.5f);
    textureCoords.emplace_back(0.0f, 0.0f);

    positions.emplace_back(-0.5f, 0.5f, 0.5f);
    textureCoords.emplace_back(1.0f, 0.0f);

    positions.emplace_back(-0.5f, 0.5f, -0.5f);
    textureCoords.emplace_back(1.0f, 1.0f);

    positions.emplace_back(-0.5f, -0.5f, -0.5f);
    textureCoords.emplace_back(0.0f, 1.0f);

    positions.emplace_back(-0.5f, -0.5f, -0.5f);
    textureCoords.emplace_back(0.0f, 1.0f);

    positions.emplace_back(-0.5f, -0.5f, 0.5f);
    textureCoords.emplace_back(0.0f, 0.0f);

    positions.emplace_back(-0.5f, 0.5f, 0.5f);
    textureCoords.emplace_back(1.0f, 0.0f);

    positions.emplace_back(0.5f, 0.5f, 0.5f);
    textureCoords.emplace_back(1.0f, 0.0f);

    positions.emplace_back(0.5f, 0.5f, -0.5f);
    textureCoords.emplace_back(1.0f, 1.0f);

    positions.emplace_back(0.5f, -0.5f, -0.5f);
    textureCoords.emplace_back(0.0f, 1.0f);

    positions.emplace_back(0.5f, -0.5f, -0.5f);
    textureCoords.emplace_back(0.0f, 1.0f);

    positions.emplace_back(0.5f, -0.5f, 0.5f);
    textureCoords.emplace_back(0.0f, 0.0f);

    positions.emplace_back(0.5f, 0.5f, 0.5f);
    textureCoords.emplace_back(1.0f, 0.0f);

    positions.emplace_back(-0.5f, -0.5f, -0.5f);
    textureCoords.emplace_back(0.0f, 1.0f);

    positions.emplace_back(0.5f, -0.5f, -0.5f);
    textureCoords.emplace_back(1.0f, 1.0f);

    positions.emplace_back(0.5f, -0.5f, 0.5f);
    textureCoords.emplace_back(1.0f, 0.0f);

    positions.emplace_back(0.5f, -0.5f, 0.5f);
    textureCoords.emplace_back(1.0f, 0.0f);

    positions.emplace_back(-0.5f, -0.5f, 0.5f);
    textureCoords.emplace_back(0.0f, 0.0f);

    positions.emplace_back(-0.5f, -0.5f, -0.5f);
    textureCoords.emplace_back(0.0f, 1.0f);

    positions.emplace_back(-0.5f, 0.5f, -0.5f);
    textureCoords.emplace_back(0.0f, 1.0f);

    positions.emplace_back(0.5f, 0.5f, -0.5f);
    textureCoords.emplace_back(1.0f, 1.0f);

    positions.emplace_back(0.5f, 0.5f, 0.5f);
    textureCoords.emplace_back(1.0f, 0.0f);

    positions.emplace_back(0.5f, 0.5f, 0.5f);
    textureCoords.emplace_back(1.0f, 0.0f);

    positions.emplace_back(-0.5f, 0.5f, 0.5f);
    textureCoords.emplace_back(0.0f, 0.0f);

    positions.emplace_back(-0.5f, 0.5f, -0.5f);
    textureCoords.emplace_back(0.0f, 1.0f);

    auto& mesh{_registry.emplace<Mesh>(entity, std::move(positions), std::move(textureCoords))};
    LoadModelHelpers::setupMesh(mesh);

    auto& textureList{_registry.emplace<TextureList>(entity)};

    Texture texture1{};
    //Texture texture2{};
    texture1.CreateTexture("../../Assets/container.jpg");
    //texture2.CreateTexture("../../Assets/awesomeface.jpg");

    textureList.TexturesDiffuse.emplace_back(texture1);

    //textureList.TexturesDiffuse.emplace_back(texture2);
}
