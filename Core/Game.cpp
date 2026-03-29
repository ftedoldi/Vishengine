#include "Game.h"

#include "Camera/CameraFactory.h"
#include "Components/Light.h"
#include "Components/Lights/PointLight.h"
#include "Components/MeshNodeTag.h"
#include "ModelLoader.h"

#include "Components/Camera/ActiveCameraTag.h"
#include "Components/Lights/DirectionalLight.h"
#include "Components/Transforms/RelativeTransform.h"
#include "DataStructures/Octree.h"
#include "Libs/assimp/code/AssetLib/MMD/MMDCpp14.h"
#include "Platform/Framebuffer.h"
#include "Platform/Time.h"

#include "Systems/DebugRenderPass.h"
#include "Systems/SceneRenderPass.h"

#include <filesystem>

Game::Game() {
    _window = std::make_unique<Window>();
    _window->Initialize();

    const auto editorCamera{CameraFactory::CreateEditorCamera(
        _registry,
        glm::vec3{0., 0., 0.},
        45.,
        static_cast<float>(_window->GetWidth()) / static_cast<float>(_window->GetHeight()),
        0.1,
        100.,
        CameraType::Perspective)};

    _registry.emplace<ActiveCameraTag>(editorCamera);

    _cameraProjectionUpdaterSystem = std::make_unique<CameraProjectionUpdaterSystem>(_registry, _window->GetEventDispatcher());

    const std::string shadersBasePath{std::string(PROJECT_SOURCE_DIR) + "/Shaders/GlslShaders/"};

    // The scene is rendered into this offscreen framebuffer.
    const auto sceneFrameBuffer{std::make_shared<Framebuffer>(0, 0, _window->GetWidth(), _window->GetHeight())};
    _rendererSystem = std::make_unique<RendererSystem>(sceneFrameBuffer, _window->GetEventDispatcher());

    const auto meshController{std::make_shared<MeshController>()};
    const auto materialController{std::make_shared<MaterialController>()};

    auto mainShader{std::make_unique<Shader>(shadersBasePath + "vertex.glsl", shadersBasePath + "fragment.glsl")};
    _rendererSystem->AddPass(std::make_unique<SceneRenderPass>(std::move(mainShader), _registry, materialController, meshController));

    _inputManager = std::make_shared<InputManager>(_window->GetGLFWwindow());

    // Debug pass

    _editorCameraMoveSystem = std::make_unique<EditorCameraMoveSystem>(_inputManager);

    const std::filesystem::path assetsRoot{std::string(PROJECT_SOURCE_DIR) + "/Assets"};
    _guiDrawer = std::make_unique<GUIDrawer>(_window->GetGLFWwindow(), sceneFrameBuffer, assetsRoot);

    ModelLoader modelLoader{_registry, meshController, materialController};
    modelLoader.ImportModel(std::string(PROJECT_SOURCE_DIR) + "/Assets/hierarchy.glb");

    // Build the octree and fill it.
    //_octreeRootNode = Octree::Build(_registry);

    auto debugShader{std::make_unique<Shader>(shadersBasePath + "debug_vertex.glsl", shadersBasePath + "debug_fragment.glsl")};
    _rendererSystem->AddPass(std::make_unique<DebugRenderPass>(_octreeRootNode.get(), _inputManager.get(), std::move(debugShader), _registry));

    // TODO: transform system should just fire an event when the transform of a specific entity is changed. It should not take an octree as input
    _transformSystem = std::make_unique<TransformSystem>(_octreeRootNode.get());

    _addLight();
}

void Game::Update() {
    while (!_window->ShouldWindowClose()) {
        glfwPollEvents();

        Time::UpdateDeltaTime();

        // Update entity transforms and camera view matrix.
        _transformSystem->Update(_registry);
        _cameraSystem.Update(_registry);
        _editorCameraMoveSystem->Update(Time::GetDeltaTime(), _registry);

        // ── Render frame ──────────────────────────────────────────────────
        // 1. Clear the default framebuffer and start the ImGui frame.
        _guiDrawer->BeginFrame();

        // 2. Render the scene into the offscreen framebuffer.
        _rendererSystem->Update();

        // 3. Build ImGui panel draw-lists (ScenePanel reads the now-populated
        //    offscreen framebuffer texture).
        _guiDrawer->DrawUI(_registry);

        // 4. Submit ImGui draw-lists to the default framebuffer.
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

    auto& relativeTransform{_registry.emplace<RelativeTransform>(pointLightEntity)};
    relativeTransform.Value.Position = {0.f, 6.f, 0.f};

    const auto dirLightEntity{_registry.create()};
    auto& dirLight{_registry.emplace<DirectionalLight>(dirLightEntity)};
    dirLight.Direction = {0.f, -1.f, 0.f};
    dirLight.Ambient   = {.6f, .6f, .6f};
    dirLight.Diffuse   = {0.5, 0.5, 0.5};
    dirLight.Specular  = {1.f, 1.f, 1.f};
}
