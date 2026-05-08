#pragma once

#include "Controllers/MaterialController.h"
#include "Controllers/MeshController.h"
#include "Controllers/ShadersController.h"
#include "Controllers/FramebuffersController.h"
#include "DataStructures/Octree.h"
#include "GUIDrawer.h"
#include "InputManager.h"
#include "Systems/CameraProjectionUpdaterSystem.h"
#include "Systems/CameraSystem.h"
#include "Systems/EditorCameraMoveSystem.h"
#include "Systems/PickingSystem.h"
#include "Systems/RendererSystem.h"
#include "Systems/SpatialSystem.h"
#include "Systems/TransformSystem.h"
#include "Window.h"

#include <entt/entt.hpp>

#include <memory>

class Game {
public:
    Game();

    void Update();

private:
    void _initWindow();

    void _initRenderer();

    void _initSystems();

    void _loadDefaultScene();

    void _initEditor();

    void _addLight();

    entt::registry _registry{};

    std::unique_ptr<MeshController> _meshController{};

    std::unique_ptr<MaterialController> _materialController{};

    std::unique_ptr<ShadersController> _shadersController{};

    std::unique_ptr<FramebuffersController> _framebuffersController{};

    std::unique_ptr<CameraProjectionUpdaterSystem> _cameraProjectionUpdaterSystem{};

    std::unique_ptr<Window> _window{};

    std::unique_ptr<RendererSystem> _rendererSystem{};

    std::shared_ptr<InputManager> _inputManager{};

    std::unique_ptr<EditorCameraMoveSystem> _editorCameraMoveSystem{};

    std::unique_ptr<GUIDrawer> _guiDrawer{};

    std::unique_ptr<Octree> _octree{};

    std::unique_ptr<TransformSystem> _transformSystem{};

    std::unique_ptr<SpatialSystem> _spatialSystem{};

    std::unique_ptr<PickingSystem> _pickingSystem{};

    std::unique_ptr<CameraSystem> _cameraSystem{};

    entt::dispatcher _dispatcher{};

    std::vector<entt::entity> _renderingPasses{};
};
