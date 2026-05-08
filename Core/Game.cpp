#include "Game.h"

#include "Camera/CameraFactory.h"
#include "Components/BoundingBox.h"
#include "Components/Camera/ActiveCameraTag.h"
#include "Components/Light.h"
#include "Components/Lights/DirectionalLight.h"
#include "Components/Lights/PointLight.h"
#include "Components/Relationship.h"
#include "Components/RenderingComponents.h"
#include "Components/Transforms/RelativeTransform.h"
#include "Components/Transforms/TransformDirtyFlag.h"
#include "Components/Transforms/WorldTransform.h"
#include "DataStructures/Octree.h"
#include "ModelLoader.h"
#include "Platform/Framebuffer.h"
#include "Platform/Time.h"
#include "RenderingComponents/LineDrawer.h"

#include <filesystem>

Game::Game() {
    _window = std::make_unique<Window>(_dispatcher);
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

    _cameraProjectionUpdaterSystem = std::make_unique<CameraProjectionUpdaterSystem>(_registry, _dispatcher);

    const std::string shadersBasePath{std::string(PROJECT_SOURCE_DIR) + "/Shaders/GlslShaders/"};

    // TODO: change with a UUID
    _framebuffersController = std::make_unique<FramebuffersController>();
    _framebuffersController->AddFramebuffer(FramebufferID::Main,
        std::make_unique<Framebuffer>(0, 0, _window->GetWidth(), _window->GetHeight()));

    // Register the lit shader.
    _shadersController = std::make_unique<ShadersController>();
    // TODO: change with a UUID
    auto litShader{std::make_unique<Shader>(shadersBasePath + "vertex.glsl", shadersBasePath + "fragment.glsl")};
    _shadersController->AddShader(ShaderID::Standard, std::move(litShader));

    // Promote the editor camera into the main view: it now owns the framebuffer
    // and shader binding for the scene pass.
    auto& editorCameraRenderTarget{_registry.emplace<RenderTarget>(editorCamera)};
    editorCameraRenderTarget.FramebufferHandle = FramebufferID::Main;

    std::bitset<32> layers{};
    layers.set(static_cast<size_t>(RenderLayer::SceneMeshes));

    const RenderPass editorCameraRenderPass{.ShaderHandle = ShaderID::Standard, .RenderLayers = {layers}, .Meshes = MeshSet::Visible};
    editorCameraRenderTarget.Passes.push_back(editorCameraRenderPass);

    _registry.emplace<LitPassTag>(editorCamera);

    _meshController = std::make_unique<MeshController>();
    _materialController = std::make_unique<MaterialController>();
    _rendererSystem = std::make_unique<RendererSystem>(_dispatcher, _materialController.get(), _meshController.get(), _shadersController.get(), _framebuffersController.get());

    _inputManager = std::make_shared<InputManager>(_window->GetGLFWWindow());

    _editorCameraMoveSystem = std::make_unique<EditorCameraMoveSystem>(_inputManager);

    const std::filesystem::path assetsRoot{std::string(PROJECT_SOURCE_DIR) + "/Assets"};
    _guiDrawer = std::make_unique<GUIDrawer>(_window->GetGLFWWindow(), _framebuffersController.get(), assetsRoot);

    _transformSystem = std::make_unique<TransformSystem>(_dispatcher);

    ModelLoader modelLoader{_registry, _meshController.get(), _materialController.get()};
    modelLoader.ImportModel(std::string(PROJECT_SOURCE_DIR) + "/Assets/megaHierarchy.glb");
    _transformSystem->Init(_registry);

    _octree = std::make_unique<Octree>();

    _spatialSystem = std::make_unique<SpatialSystem>(_octree.get(), _registry, _dispatcher);
    _spatialSystem->Init();

    // Initialize frustum debug view framebuffer and shader.
    _framebuffersController->AddFramebuffer(FramebufferID::FrustumDebugView, std::make_unique<Framebuffer>(0, 0, _window->GetWidth(), _window->GetHeight()));
    auto frustumDebugShader{std::make_unique<Shader>(shadersBasePath + "debug_vertex.glsl", shadersBasePath + "debug_fragment.glsl")};
    _shadersController->AddShader(ShaderID::FrustumDebug, std::move(frustumDebugShader));

    _pickingSystem = std::make_unique<PickingSystem>(_window.get(), _octree.get(), _dispatcher, _registry);

    LineDrawer::Initialize(_registry);

    _addLight();
}

void Game::Update() {
    while (!_window->ShouldWindowClose()) {
        glfwPollEvents();
        _inputManager->Update();

        Time::UpdateDeltaTime();

        // Update entity transforms and camera view matrix.
        _transformSystem->Update(_registry);

        _cameraSystem.Update(_registry);

        _editorCameraMoveSystem->Update(Time::GetDeltaTime(), _registry);

        _spatialSystem->Update(_registry);

        // ── Render frame ──────────────────────────────────────────────────
        // 1. Clear the default framebuffer and start the ImGui frame.
        _guiDrawer->BeginFrame();

        // 2. Render the scene into the offscreen framebuffer.
        _rendererSystem->Update(_registry);
        _pickingSystem->DrawPickingRay();

        // 3. Build ImGui panel draw-lists (ScenePanel reads the now-populated
        //    offscreen framebuffer texture).
        _guiDrawer->DrawUI(_dispatcher, _registry);

        // 4. Submit ImGui draw-lists to the default framebuffer.
        _guiDrawer->Render();

        _window->Update();
    }
}

void Game::_addLight() {
    const auto pointLightEntity{_registry.create()};
    auto& light{_registry.emplace<PointLight>(pointLightEntity)};
    light.Diffuse  = {1.0, 1.0, 1.0};
    light.Ambient  = {0.2, 0.2, 0.2};
    light.Specular = {1.f, 1.f, 1.f};

    _registry.emplace<BoundingBox>(pointLightEntity, Box{glm::vec3{-0.15f}, glm::vec3{0.15f}});
    auto& relativeTransform{_registry.emplace<RelativeTransform>(pointLightEntity)};
    _registry.emplace<WorldTransform>(pointLightEntity);
    _registry.emplace<TransformDirtyFlag>(pointLightEntity);
    _registry.emplace<Relationship>(pointLightEntity);
    relativeTransform.Value.Position = {0.f, 6.f, 0.f};

    const auto dirLightEntity{_registry.create()};
    auto& dirLight{_registry.emplace<DirectionalLight>(dirLightEntity)};
    dirLight.Direction = {0.f, -1.f, 0.f};
    dirLight.Ambient   = {.6f, .6f, .6f};
    dirLight.Diffuse   = {0.5, 0.5, 0.5};
    dirLight.Specular  = {1.f, 1.f, 1.f};
}
