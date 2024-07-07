#include "EntityManager.h"

#include "Components/EventComponents/FramebufferSizeEventComponent.h"
#include "Components/EventComponents/KeyPressedEventComponent.h"
#include "Components/EventComponents/MouseMovedEventComponent.h"
#include "Components/Mesh.h"
#include "Time/Time.h"

EntityManager::EntityManager() : _editorCamera{_registry.create()} {
    _windowHandlerSystem = std::make_unique<WindowHandlerSystem>(_registry);
    _windowHandlerSystem->InitializeWindow();

    _mainShader = std::make_unique<Shader>("../../Shaders/GlslShaders/vertex.glsl", "../../Shaders/GlslShaders/fragment.glsl");

    _createMeshSystem = std::make_unique<CreateMeshSystem>(_registry);
    _drawMeshesSystem = std::make_unique<DrawMeshesSystem>(_registry, _mainShader.get(), _editorCamera);

    auto& camera{_registry.emplace<Camera>(_editorCamera)};
    auto& transform{_registry.emplace<Transform>(_editorCamera)};

    transform.Rotation = glm::quatLookAtRH(camera.Front, camera.Up);
    transform.Translation = camera.Position;

    auto& keyPressedEventComponent{_registry.emplace<KeyPressedEventComponent>(_editorCamera)};
    keyPressedEventComponent.OnKeyPressed = [this, &camera, &transform](GLFWwindow*, int key, int, int, int){
        switch (key) {
            case GLFW_KEY_W:
                camera.Position.y = _cameraYPosition;
                camera.Position += camera.Front * Camera::CameraSpeed * Time::GetDeltaTime();
                transform.Translation = camera.Position;
                break;
            case GLFW_KEY_S:
                camera.Position.y = _cameraYPosition;
                camera.Position -= camera.Front * Camera::CameraSpeed * Time::GetDeltaTime();
                transform.Translation = camera.Position;
                break;
            case GLFW_KEY_A:
                camera.Position -= glm::normalize(glm::cross(camera.Front, camera.Up)) * Camera::CameraSpeed * Time::GetDeltaTime();
                transform.Translation = camera.Position;
                break;
            case GLFW_KEY_D:
                camera.Position += glm::normalize(glm::cross(camera.Front, camera.Up)) * Camera::CameraSpeed * Time::GetDeltaTime();
                transform.Translation = camera.Position;
                break;
            case GLFW_KEY_E:
                camera.Position += glm::vec3{0, 1, 0} * Camera::CameraSpeed * Time::GetDeltaTime();
                transform.Translation = camera.Position;
                _cameraYPosition = camera.Position.y;
                break;
            case GLFW_KEY_Q:
                camera.Position -= glm::vec3{0, 1, 0} * Camera::CameraSpeed * Time::GetDeltaTime();
                transform.Translation = camera.Position;
                _cameraYPosition = camera.Position.y;
                break;
            default: break;
        };
    };

    auto& mouseMovedEventComponent{_registry.emplace<MouseMovedEventComponent>(_editorCamera)};
    mouseMovedEventComponent.OnMouseMoved = [&camera, &transform](GLFWwindow*, double xPos, double yPos){
        if (camera.FirstTimeMovingMouse){
            camera.LastX = xPos;
            camera.LastY = yPos;
            camera.FirstTimeMovingMouse = false;
        }

        double xOffset{xPos - camera.LastX};
        double yOffset{camera.LastY - yPos};

        camera.LastX = xPos;
        camera.LastY = yPos;

        const float sensitivity{0.1f};
        xOffset *= sensitivity;
        yOffset *= sensitivity;

        camera.CameraYaw += xOffset;
        camera.CameraPitch += yOffset;

        if(camera.CameraPitch > 89.0)
            camera.CameraPitch = 89.0;
        if(camera.CameraPitch < -89.0)
            camera.CameraPitch = -89.0;

        const auto qx{glm::angleAxis(static_cast<float>(glm::radians(camera.CameraPitch / 2.f)), glm::vec3{1.0, 0.0, 0.0})};
        const auto qy{glm::angleAxis(static_cast<float>(glm::radians(-camera.CameraYaw / 2.f)), glm::vec3{0.0, 1.0, 0.0})};
        const auto q{qy * qx};

        camera.Front = glm::normalize(q * glm::vec3{0.0, 0.0, -1.0});
        camera.Up = glm::normalize(q * glm::vec3{0.0, 1.0, 0.0});

        transform.Rotation = glm::quatLookAtRH(camera.Front, camera.Up);
    };

    auto& framebufferSizeEventComponent{_registry.emplace<FramebufferSizeEventComponent>(_editorCamera)};
    framebufferSizeEventComponent.OnFramebufferSizeChanged = [&camera](GLFWwindow*, int width, int height){
        camera.PerspectiveMatrix = glm::perspective(glm::radians(45.f), static_cast<float>(width) / static_cast<float>(height), 0.1f, 100.f);
    };

    // Setup the mesh

    _createMeshSystem->CreateMesh();
}

void EntityManager::Update() {
    while(!_windowHandlerSystem->ShouldWindowClose()) {

        WindowHandlerSystem::Clear();

        Time::UpdateDeltaTime();

        _drawMeshesSystem->Update(Time::GetDeltaTime());

        _windowHandlerSystem->Update();
    }
}

void EntityManager::Clear() {
    _mainShader->DeleteProgram();
    glfwTerminate();
}
