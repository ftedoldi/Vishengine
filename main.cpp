#include "Shaders/Shader.h"
#include "Window/Window.h"
#include "Texture/Texture.h"

#include "EntityComponent/Mesh.h"
#include "EntityComponent/Entity.h"
#include "EntityComponent/EntityManager.h"

#include "Time/Time.h"

#include "Camera/Camera.h"

#include <glm/glm.hpp>

int main(){
    Window windowHandler;

    InputManager inputManager = windowHandler.CreateInputManager();

    Camera camera{&windowHandler};

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

    std::string vertexPath = "../../Shaders/vertex.glsl";
    std::string fragmentPath = "../../Shaders/fragment.glsl";

    Shader shader(vertexPath, fragmentPath);

    EntityManager entityManager;
    Entity entity;

    auto mesh{make_unique<Mesh>(vertices, indices)};

    mesh->_camera = &camera;
    mesh->_shader = &shader;

    entity.AddComponent(std::move(mesh));

    entityManager.AddEntity(std::move(entity));

    entityManager.StartEntities();

    Texture tex1;
    tex1.createTexture("../../Assets/container.jpg");

    Texture tex2;
    tex2.createTexture("../../Assets/awesomeface.jpg");

    shader.UseProgram();
    shader.SetUniformInt("texture1", 0);
    shader.SetUniformInt("texture2", 1);

    while (!windowHandler.ShouldWindowClose()) {
        Window::Clear();

        Time::UpdateDeltaTime();

        tex1.bindTexture(0);
        tex2.bindTexture(1);

        shader.UseProgram();

        camera.ProcessInput(inputManager);

        shader.SetUniformMat4("Perspective", camera.GetPerspectiveMatrix());
        
        entityManager.UpdateEntities();

        windowHandler.Update();
    }
    shader.DeleteProgram();

    glfwTerminate();
    return 0;
}