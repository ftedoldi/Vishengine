#include "Game.h"

#include "Components/Light.h"
#include "Components/Lights/PointLight.h"

#include "Components/Camera/ActiveCameraTag.h"
#include "Components/Lights/DirectionalLight.h"
#include "Platform/Time.h"

#include <array>

Game::Game() {
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
    const std::string shadersBasePath{std::string(PROJECT_SOURCE_DIR) + "/Shaders/GlslShaders/"};
    _mainShader = std::make_unique<Shader>(shadersBasePath + "vertex.glsl", shadersBasePath + "fragment.glsl");

    _rendererSystem = std::make_unique<RendererSystem>(_mainShader.get());

    _inputManager = std::make_unique<InputManager>(_window->GetGLFWwindow());

    _editorCameraMoveSystem = std::make_unique<EditorCameraMoveSystem>(_inputManager.get());

    _guiDrawer = std::make_unique<GUIDrawer>(_window->GetGLFWwindow());

    ModelLoader modelLoader{_registry, _meshController, _materialController};
    std::optional<entt::entity> entity{};
    const std::array<std::string, 2> candidateModelPaths{
        std::string(PROJECT_SOURCE_DIR) + "/Assets/pimmyTex.glb",
        std::string(PROJECT_SOURCE_DIR) + "/cubi.glb"
    };

    for (const auto& modelPath : candidateModelPaths) {
        entity = modelLoader.ImportModel(modelPath);
        if (entity.has_value()) {
            break;
        }
    }

    //auto& position{_registry.get<Position>(entity.value())};

    //position.Vector = glm::vec3{0, 10, 0};
    //auto entity{modelLoader.ImportModel("../../Assets/Backpack/backpack.obj")};
    assert(entity && "No default model found. Add cubi.glb to Assets/ or repository root.");

    _addLight();

    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
}

void Game::Update() {
    while(!_window->ShouldWindowClose()) {
        glfwPollEvents();

        _guiDrawer->StartFrame(_registry);

        Window::Clear();

        Time::UpdateDeltaTime();

        // Update the entity transforms
        _transformSystem.Update(_registry);

        // Update the view transform
        _cameraSystem.Update(_registry);

        _rendererSystem->Update(Time::GetDeltaTime(), _registry, _materialController, _meshController);

        _editorCameraMoveSystem->Update(Time::GetDeltaTime(), _registry);

        _guiDrawer->Render();
        // Last (glfwSwapBuffers())
        _window->Update();
    }
}

void Game::Clear() {
    _mainShader->DeleteProgram();
    _guiDrawer->Clear();
}

void Game::_addLight() {
    const auto pointLightEntity{_registry.create()};
    auto& light{_registry.emplace<PointLight>(pointLightEntity)};
    light.Diffuse = {1.0, 1.0, 1.0};
    light.Ambient = {0.2, 0.2, 0.2};
    light.Specular = {1.f, 1.f, 1.f};

    auto& position{_registry.emplace<Position>(pointLightEntity)};
    position.Vector = {0.f, 6.f, 0.f};

    const auto dirLightEntity{_registry.create()};

    auto& dirLight{_registry.emplace<DirectionalLight>(dirLightEntity)};
    dirLight.Direction = {0.f, -1.f, 0.f};
    dirLight.Ambient = {.1f, .1f, .1f};
    dirLight.Diffuse = {0.5, 0.5, 0.5};
    dirLight.Specular = {1.f, 1.f, 1.f};
}
