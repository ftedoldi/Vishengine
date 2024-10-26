#pragma once

#include "Components/CameraComponents/Camera.h"
#include "InputManager/InputManager.h"
#include "Shaders/Shader.h"

#include "Systems/CameraMoveSystem.h"
#include "Systems/CreateCameraSystem.h"
#include "Systems/ImGuiHandlerSystem.h"
#include "Systems/RendererSystem.h"
#include "Systems/WindowHandlerSystem.h"

#include <entt/entt.hpp>

#include <memory>

class EntityManager {
public:
    EntityManager();

    void Update();

    void Clear();

private:
    void _setupEditorCamera();

    void _addLight();

    void _addPointLight();

    // TODO: check if maybe it's needed more than a generic registry for all the entities
    entt::registry _registry;

    entt::entity _editorCamera;

    entt::entity _mainWindow;

    std::unique_ptr<Shader> _mainShader;

    std::unique_ptr<RendererSystem> _drawMeshesSystem;
    std::unique_ptr<WindowHandlerSystem> _windowHandlerSystem;
    std::unique_ptr<InputManager> _inputManager;
    std::unique_ptr<CameraMoveSystem> _cameraMoveSystem;
    std::unique_ptr<ImGuiHandlerSystem> _imGuiHandlerSystem;
};