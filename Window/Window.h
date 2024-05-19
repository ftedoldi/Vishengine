#pragma once

#include "Event/Event.h"

#include <glad/gl.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include <string>
#include <memory>
#include <functional>

DECLARE_EVENT(KeyboardKeyPressed, int, int);

class Window {
public:
    Window();

    Window(int width, int height, const std::string& windowName);

	bool ShouldWindowClose();
	void Update();

    static void Clear();

    KeyboardKeyPressed& OnKeyboardKeyPressed();

private:
	GLFWwindow* _window{nullptr};

    KeyboardKeyPressed _onKeyboardKeyPressed;

    void _initializeWindow(int width = 800, int height = 600, const std::string& windowName = "VishEngine");
	void _setFramebufferSizeCallback();

    void _setKeyPressedCallback();
};