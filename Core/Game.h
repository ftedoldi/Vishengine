#pragma once

#include "Window.h"
#include "InputManager.h"
#include "GUIDrawer.h"
#include "Platform/Framebuffer.h"
#include "Systems/CameraProjectionUpdaterSystem.h"
#include "Systems/CameraSystem.h"
#include "Systems/EditorCameraMoveSystem.h"
#include "Systems/RendererSystem.h"
#include "Systems/TransformSystem.h"

#include <entt/entt.hpp>

#include <memory>

class Game {
public:
    Game();

    void Update();

    void Clear() const;

private:
    void _addLight();

    entt::registry _registry{};

    std::unique_ptr<CameraProjectionUpdaterSystem> _cameraProjectionUpdaterSystem{};

    std::unique_ptr<Window> _window{};

    std::unique_ptr<RendererSystem> _rendererSystem{};

    std::shared_ptr<InputManager> _inputManager{};

    std::unique_ptr<EditorCameraMoveSystem> _editorCameraMoveSystem{};

    std::unique_ptr<GUIDrawer> _guiDrawer{};

    std::shared_ptr<Framebuffer> _sceneFrameBuffer{};

    TransformSystem _transformSystem{};

    CameraSystem _cameraSystem{};
};
