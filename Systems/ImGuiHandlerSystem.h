#pragma once

#include <entt/entt.hpp>

class ImGuiHandlerSystem {
public:
    ImGuiHandlerSystem(entt::registry& registry, entt::entity windowEntity);

    void StartFrame();

    void Render();

    void Clear();

private:
    entt::registry& _registry;

    entt::entity _window;
};
