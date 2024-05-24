#pragma once

#include "Event/Event.h"
#include "Input/InputManager.h"

#include <glad/gl.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include <string>
#include <memory>
#include <functional>

DECLARE_EVENT(KeyboardKeyPressed, GLFWwindow*, int, int);

DECLARE_EVENT(FramebufferSizeChanged, int, int);

DECLARE_EVENT(MouseMoved, double, double);

class Window {
public:
    Window();

    ~Window();

    Window(int width, int height, const std::string& windowName);

	bool ShouldWindowClose() const;
	void Update() const;

    int GetWidth() const;

    int GetHeight() const;

    static void Clear();

    FramebufferSizeChanged& OnFramebufferSizeChanged();

    MouseMoved& OnMouseMoved();

    InputManager CreateInputManager();

private:
	GLFWwindow* _window{nullptr};

    KeyboardKeyPressed _onKeyboardKeyPressed;
    FramebufferSizeChanged _onFramebufferSizeChanged;
    MouseMoved _onMouseMoved;

    int _width{0};
    int _height{0};

    void _initializeWindow(int width = 800, int height = 600, const std::string& windowName = "VishEngine");

	void _setFramebufferSizeCallback() const;
    void _setKeyPressedCallback() const;
    void _setMouseMovedCallback() const;
};