#pragma once

#include "EntityManager.h"

class Game {
public:
    Game();

private:
    EntityManager _entityManager;

    // TODO: check if maybe it's needed more than a generic registry for all the entities
    entt::registry _registry;

    entt::entity _editorCamera;

    Shader _mainShader{"../../../Shaders/vertex.glsl", "../../../Shaders/fragment.glsl"};
};
