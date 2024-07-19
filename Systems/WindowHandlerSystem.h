#pragma once

#include "Components/Window.h"
#include "Components/EventComponents/KeyPressedEventComponent.h"

#include <entt/entt.hpp>

#include <string>

class WindowHandlerSystem {
public:
    explicit WindowHandlerSystem(entt::registry& registry, entt::entity mainWindow);

    void InitializeWindow(int width = 800, int height = 600, const std::string& windowName = "VishEngine");

    bool ShouldWindowClose() const;
    void Update() const;

    static void Clear();

private:
    entt::registry& _registry;
    Window* _window;
    KeyPressedEventComponent* _keyPressedEvent;

    void _setFramebufferSizeCallback() const;
    void _setKeyPressedCallback() const;
    void _setMouseMovedCallback() const;
};
