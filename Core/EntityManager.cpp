#include "EntityManager.h"

#include "Components/EventComponents/FramebufferSizeEventComponent.h"
#include "Components/EventComponents/KeyPressedEventComponent.h"
#include "Components/EventComponents/MouseMovedEventComponent.h"
#include "Components/Mesh.h"
#include "Components/CameraComponents/Perspective.h"
#include "Components/Light.h"

#include "Systems/LoadModelSystem.h"

#include "Platform/Time.h"
#include "Platform/Mouse.h"

EntityManager::EntityManager() : _mainWindow{_registry.create()} {
    // Setup window
    _windowHandlerSystem = std::make_unique<WindowHandlerSystem>(_registry, _mainWindow);
    _windowHandlerSystem->InitializeWindow();

    // Create and setup editor camera.
    CreateCameraSystem createCameraSystem{_registry, _mainWindow};
    _editorCamera = createCameraSystem.CreateCamera(CameraType::Perspective);
    _setupEditorCamera();

    // Create shader.
    _mainShader = std::make_unique<Shader>("../../Shaders/GlslShaders/vertex.glsl", "../../Shaders/GlslShaders/fragment.glsl");

    _drawMeshesSystem = std::make_unique<RendererSystem>(_registry, _mainShader.get(), _editorCamera);

    _inputManager = std::make_unique<InputManager>(_registry, _mainWindow);

    _cameraMoveSystem = std::make_unique<CameraMoveSystem>(_registry, _editorCamera, _inputManager.get());

    _imGuiHandlerSystem = std::make_unique<ImGuiHandlerSystem>(_registry, _mainWindow);

    LoadModelSystem loadModelSystem{_registry};
    loadModelSystem.ImportModel("../../Assets/hierarchy.fbx");
    //loadModelSystem.ImportModel("../../Assets/planeTest.obj");

    _addLight();

    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
}

void EntityManager::Update() {
    while(!_windowHandlerSystem->ShouldWindowClose()) {
        glfwPollEvents();

        _imGuiHandlerSystem->StartFrame();

        WindowHandlerSystem::Clear();

        Time::UpdateDeltaTime();

        _drawMeshesSystem->Update(Time::GetDeltaTime());

        _cameraMoveSystem->Update(Time::GetDeltaTime());

        _imGuiHandlerSystem->Render();
        // Last (glfwSwapBuffers())
        _windowHandlerSystem->Update();
    }
}

void EntityManager::Clear() {
    _mainShader->DeleteProgram();
    //_imGuiHandlerSystem->Clear();

    glfwTerminate();
}

void EntityManager::_setupEditorCamera() {
    // Setup editor camera mouse rotation.
    auto& camera{_registry.get<Camera>(_editorCamera)};
    auto& rotation{_registry.get<Rotation>(_editorCamera)};

    auto& mouseMovedEventComponent{_registry.emplace<MouseMovedEventComponent>(_editorCamera)};
    mouseMovedEventComponent.OnMouseMoved = [&camera, &rotation](GLFWwindow*, double xPos, double yPos){
        if (Mouse::FirstTimeMovingMouse){
            Mouse::LastXPos = xPos;
            Mouse::LastYPos = yPos;
            Mouse::FirstTimeMovingMouse = false;
        }

        double xOffset{xPos - Mouse::LastXPos};
        double yOffset{Mouse::LastYPos - yPos};

        Mouse::LastXPos = xPos;
        Mouse::LastYPos = yPos;

        const float sensitivity{0.1f};
        xOffset *= sensitivity;
        yOffset *= sensitivity;

        camera.YawAngle += xOffset;
        camera.PitchAngle += yOffset;

        if(camera.PitchAngle > 179.0)
            camera.PitchAngle = 179.0;
        if(camera.PitchAngle < -179.0)
            camera.PitchAngle = -179.0;

        const auto qx{glm::angleAxis(static_cast<float>(glm::radians(camera.PitchAngle / 2.f)), glm::vec3{1.0, 0.0, 0.0})};
        const auto qy{glm::angleAxis(static_cast<float>(glm::radians(-camera.YawAngle / 2.f)), glm::vec3{0.0, 1.0, 0.0})};
        const auto q{qy * qx};

        camera.Front = glm::normalize(q * glm::vec3{0.0, 0.0, -1.0});
        camera.Up = glm::normalize(q * glm::vec3{0.0, 1.0, 0.0});

        rotation.Quaternion = glm::quatLookAtRH(camera.Front, camera.Up);
    };

    // Setup editor camera framebuffer size changed event.
    auto& framebufferSizeEventComponent{_registry.emplace<FramebufferSizeEventComponent>(_editorCamera)};

    auto& perspective{_registry.get<Perspective>(_editorCamera)};

    framebufferSizeEventComponent.OnFramebufferSizeChanged = [&perspective](GLFWwindow*, int width, int height){
        perspective.Matrix = glm::perspective(glm::radians(45.f), static_cast<float>(width) / static_cast<float>(height), 0.1f, 100.f);
    };
}

void EntityManager::_addLight() {
    LoadModelSystem loadModelSystem{_registry};
    const auto entity{loadModelSystem.ImportModel("../../Assets/noTexCube.obj")};

    if(entity) {
        auto& light{_registry.emplace<Light>(*entity)};
        light.Diffuse = {1.0, 1.0, 1.0};

        auto& position{_registry.get<Position>(*entity)};
        position.Vector = {0.f, 8.f, -6.f};
    }
}
