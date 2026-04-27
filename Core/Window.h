#pragma once

#include "Events/WindowEvents.h"
#include "GLFW/glfw3.h"

#include <entt/entt.hpp>

#include <string>

class Window {
public:
    Window(entt::dispatcher& eventDispatcher);

    ~Window();

    void Initialize(int width = 800, int height = 600, const std::string& windowName = "VishEngine");

    [[nodiscard]] bool ShouldWindowClose() const;

    void Update() const;

    void SetWidth(int32_t width);

    void SetHeight(int32_t height);

    [[nodiscard]] int32_t GetWidth() const;

    [[nodiscard]] int32_t GetHeight() const;

    [[nodiscard]] GLFWwindow* GetGLFWWindow() const;

private:
    void _setFramebufferSizeCallback() const;

    void _setMouseButtonCallback() const;

    void _setKeyPressedCallback() const;

    void _setMouseMovedCallback() const;

    void _shouldCloseWindow(const WindowsEvents::KeyPressedEvent& keyPressedEvent) const;

    GLFWwindow* _window{};

    entt::dispatcher& _eventDispatcher;

    int32_t _width{};

    int32_t _height{};
};
