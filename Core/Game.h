#pragma once

#include "Window.h"

#include "Components/Camera/Camera.h"
#include "InputManager.h"
#include "Shaders/Shader.h"

#include "Camera/CameraFactory.h"
#include "GUIDrawer.h"
#include "ModelLoader.h"
#include "Systems/CameraProjectionUpdaterSystem.h"
#include "Systems/EditorCameraMoveSystem.h"
#include "Systems/RendererSystem.h"

#include <entt/entt.hpp>

#include <memory>

class Game {
public:
    Game();

    void Update();

    void Clear();

private:
    void _addLight();

    // TODO: check if maybe it's needed more than a generic registry for all the entities
    entt::registry _registry{};

    std::unique_ptr<CameraProjectionUpdaterSystem> _cameraProjectionUpdaterSystem{};

    std::unique_ptr<Window> _window{};

    std::unique_ptr<Shader> _mainShader{};

    std::unique_ptr<RendererSystem> _rendererSystem{};

    std::unique_ptr<InputManager> _inputManager{};

    std::unique_ptr<EditorCameraMoveSystem> _editorCameraMoveSystem{};

    std::unique_ptr<GUIDrawer> _guiDrawer{};

    MaterialController _materialController{};

    MeshController _meshController{};
};