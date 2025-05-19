#include "EntityManager.h"

#include "Components/Mesh.h"
#include "Components/Light.h"
#include "Components/Lights/PointLight.h"

#include "Systems/LoadModelSystem.h"

#include "Components/CameraComponents/ActiveCameraTag.h"
#include "Platform/Time.h"

EntityManager::EntityManager() {
    // Setup window
    _window = std::make_unique<Window>();
    _window->Initialize();

    // Create and setup editor camera.
    const auto editorCamera{CameraFactory::CreateEditorCamera(_registry,
                                      glm::vec3{0., 0., 0.},
                                      45.,
                                      _window->GetWidth() / _window->GetHeight(),
                                      0.1,
                                      100.,
                                      CameraType::Perspective)};

    _registry.emplace<ActiveCameraTag>(editorCamera);

    _cameraProjectionUpdaterSystem = std::make_unique<CameraProjectionUpdaterSystem>(_registry, _window->GetEventDispatcher());

    // Create shader.
    _mainShader = std::make_unique<Shader>("../../Shaders/GlslShaders/vertex.glsl", "../../Shaders/GlslShaders/fragment.glsl");

    _drawMeshesSystem = std::make_unique<RendererSystem>(_registry, _mainShader.get(), _editorCamera);

    _inputManager = std::make_unique<InputManager>(_window->GetGLFWwindow());

    _cameraMoveSystem = std::make_unique<CameraMoveSystem>(_registry, _editorCamera, _inputManager.get());

    _imGuiHandlerSystem = std::make_unique<ImGuiHandlerSystem>(_window->GetGLFWwindow());

    LoadModelSystem loadModelSystem{_registry};
    auto entity = loadModelSystem.ImportModel("../../Assets/ezio.fbx");

    auto& position{_registry.get<Position>(entity.value())};

    position.Vector = glm::vec3{0, 10, 0};
    //loadModelSystem.ImportModel("../../Assets/Backpack/backpack.obj");

    _addLight();

    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
}

void EntityManager::Update() {
    while(!_window->ShouldWindowClose()) {
        glfwPollEvents();

        _imGuiHandlerSystem->StartFrame(_registry);

        Window::Clear();

        Time::UpdateDeltaTime();

        _drawMeshesSystem->Update(Time::GetDeltaTime());

        _cameraMoveSystem->Update(Time::GetDeltaTime());

        _imGuiHandlerSystem->Render();
        // Last (glfwSwapBuffers())
        _window->Update();
    }
}

void EntityManager::Clear() {
    _mainShader->DeleteProgram();
    _imGuiHandlerSystem->Clear();
}

void EntityManager::_addLight() {
    const auto pointLightEntity{_registry.create()};
    auto& light{_registry.emplace<PointLight>(pointLightEntity)};
    light.Diffuse = {1.0, 1.0, 1.0};
    light.Ambient = {0.2, 0.2, 0.2};
    light.Specular = {1.f, 1.f, 1.f};

    auto& position{_registry.emplace<Position>(pointLightEntity)};
    position.Vector = {0.f, 1.f, 0.f};

    /*const auto dirLightEntity{_registry.create()};

    auto& dirLight{_registry.emplace<DirectionalLight>(dirLightEntity)};
    dirLight.Direction = {0.f, -1.f, 0.f};
    dirLight.Ambient = {.1f, .1f, .1f};
    dirLight.Diffuse = {0.5, 0.5, 0.5};
    dirLight.Specular = {1.f, 1.f, 1.f};*/
}

void EntityManager::_addPointLight() {

}
