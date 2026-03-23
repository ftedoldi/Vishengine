#include "Game.h"

#include "Camera/CameraFactory.h"
#include "Components/Light.h"
#include "Components/Lights/PointLight.h"
#include "Components/Position.h"
#include "ModelLoader.h"

#include "Components/Camera/ActiveCameraTag.h"
#include "Components/Lights/DirectionalLight.h"
#include "Platform/Framebuffer.h"
#include "Platform/Time.h"

#include "Systems/SceneRenderPass.h"
#include "Systems/ScreenBlitPass.h"

#include <array>
#include <optional>

Game::Game() {
    _window = std::make_unique<Window>();
    _window->Initialize();

    const auto editorCamera{CameraFactory::CreateEditorCamera(
        _registry,
        glm::vec3{0., 0., 0.},
        45.,
        static_cast<double>(_window->GetWidth()) / _window->GetHeight(),
        0.1,
        100.,
        CameraType::Perspective)};

    _registry.emplace<ActiveCameraTag>(editorCamera);

    _cameraProjectionUpdaterSystem = std::make_unique<CameraProjectionUpdaterSystem>(_registry, _window->GetEventDispatcher());

    const std::string shadersBasePath{std::string(PROJECT_SOURCE_DIR) + "/Shaders/GlslShaders/"};

    _rendererSystem = std::make_unique<RendererSystem>();

    const auto meshController{std::make_shared<MeshController>()};
    const auto materialController{std::make_shared<MaterialController>()};

    const auto sceneFrameBuffer{std::make_shared<Framebuffer>(0, 0, _window->GetWidth(), _window->GetHeight())};
    auto mainShader{std::make_unique<Shader>(shadersBasePath + "vertex.glsl", shadersBasePath + "fragment.glsl")};
    _rendererSystem->AddPass(std::make_unique<SceneRenderPass>(std::move(mainShader), sceneFrameBuffer, _registry, materialController, meshController, _window->GetEventDispatcher()));

    auto screenShader{std::make_unique<Shader>(shadersBasePath + "ScreenVertexShader.glsl", shadersBasePath + "ScreenFragmentShader.glsl")};
    _rendererSystem->AddPass(std::make_unique<ScreenBlitPass>(std::move(screenShader), sceneFrameBuffer));

    _inputManager = std::make_shared<InputManager>(_window->GetGLFWwindow());
    _editorCameraMoveSystem = std::make_unique<EditorCameraMoveSystem>(_inputManager);

    _guiDrawer = std::make_unique<GUIDrawer>(_window->GetGLFWwindow());

    ModelLoader modelLoader{_registry, meshController, materialController};
    std::optional<entt::entity> entity{};

    const std::array<std::string, 2> candidateModelPaths{
        std::string(PROJECT_SOURCE_DIR) + "/Assets/cubi.glb",
        std::string(PROJECT_SOURCE_DIR) + "/cubi.glb"
    };

    for (const auto& modelPath : candidateModelPaths) {
        entity = modelLoader.ImportModel(modelPath);
        if (entity.has_value()) {
            break;
        }
    }

    assert(entity && "No default model found. Add cubi.glb to Assets/ or repository root.");

    _addLight();
}

void Game::Update() {
    while (!_window->ShouldWindowClose()) {
        glfwPollEvents();

        _guiDrawer->StartFrame(_registry);

        Time::UpdateDeltaTime();

        // Update entity transforms and camera view matrix.
        _transformSystem.Update(_registry);
        _cameraSystem.Update(_registry);

        _rendererSystem->Update();

        _editorCameraMoveSystem->Update(Time::GetDeltaTime(), _registry);

        _guiDrawer->Render();

        _window->Update();
    }
}

void Game::Clear() const {
    _guiDrawer->Clear();
}

void Game::_addLight() {
    const auto pointLightEntity{_registry.create()};
    auto& light{_registry.emplace<PointLight>(pointLightEntity)};
    light.Diffuse  = {1.0, 1.0, 1.0};
    light.Ambient  = {0.2, 0.2, 0.2};
    light.Specular = {1.f, 1.f, 1.f};

    auto& position{_registry.emplace<Position>(pointLightEntity)};
    position.Vector = {0.f, 6.f, 0.f};

    const auto dirLightEntity{_registry.create()};
    auto& dirLight{_registry.emplace<DirectionalLight>(dirLightEntity)};
    dirLight.Direction = {0.f, -1.f, 0.f};
    dirLight.Ambient   = {.6f, .6f, .6f};
    dirLight.Diffuse   = {0.5, 0.5, 0.5};
    dirLight.Specular  = {1.f, 1.f, 1.f};
}
