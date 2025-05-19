#pragma once

#include "GLFW/glfw3.h"

#include <entt/entt.hpp>

#include <string>

struct FrameBufferSizeChangedEvent {
    float Width{};
    float Height{};
};

struct KeyPressedEvent {
    int Key{};
    int Scancode{};
    int Action{};
    int Mods{};
};

struct MouseMovedEvent {
    double XMousePosition{};
    double YMousePosition{};
};

class Window {
public:
    ~Window();

    void Initialize(int width = 800, int height = 600, const std::string& windowName = "VishEngine");

    bool ShouldWindowClose() const;

    void Update() const;

    entt::dispatcher& GetEventDispatcher();

    static void Clear();

    double GetHeight() const;

    double GetWidth() const;

    GLFWwindow* GetGLFWwindow();

private:
    void _setFramebufferSizeCallback() const;

    void _setKeyPressedCallback() const;

    void _setMouseMovedCallback() const;

    void _shouldCloseWindow(const KeyPressedEvent& keyPressedEvent);

    GLFWwindow* _window{};

    entt::dispatcher _eventDispatcher{};

    double _width{};

    double _height{};
};
