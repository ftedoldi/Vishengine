#pragma once

#include "Camera/Camera.h"
#include "Shaders/Shader.h"

#include "Systems/CreateMeshSystem.h"
#include "Systems/DrawMeshesSystem.h"
#include "Systems/WindowHandlerSystem.h"

#include <entt/entt.hpp>

#include <memory>

class EntityManager {
public:
    EntityManager();

    void Update();

    void Clear();

private:
    // TODO: check if maybe it's needed more than a generic registry for all the entities
    entt::registry _registry;

    entt::entity _editorCamera;

    std::unique_ptr<Shader> _mainShader;

    // TODO: ofc to change position in a "EditorCameraSystem" or something like that
    float _cameraYPosition{};

    std::unique_ptr<DrawMeshesSystem> _drawMeshesSystem;
    std::unique_ptr<CreateMeshSystem> _createMeshSystem;
    std::unique_ptr<WindowHandlerSystem> _windowHandlerSystem;
};