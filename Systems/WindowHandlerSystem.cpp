#include "WindowHandlerSystem.h"

#include "Components/EventComponents/MouseMovedEventComponent.h"
#include "Components/EventComponents/FramebufferSizeEventComponent.h"

#include <glad/gl.h>

#include <GLFW/glfw3.h>

#include <iostream>

namespace WindowSystemHelpers {
void message_callback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei, const GLchar* message, const void*){
    const auto* const src_str = [source]() {
        switch (source)
        {
            case GL_DEBUG_SOURCE_API: return "API";
            case GL_DEBUG_SOURCE_WINDOW_SYSTEM: return "WINDOW SYSTEM";
            case GL_DEBUG_SOURCE_SHADER_COMPILER: return "SHADER COMPILER";
            case GL_DEBUG_SOURCE_THIRD_PARTY: return "THIRD PARTY";
            case GL_DEBUG_SOURCE_APPLICATION: return "APPLICATION";
            case GL_DEBUG_SOURCE_OTHER: return "OTHER";
            default: return "";
        }
    }();

    const auto* const type_str = [type]() {
        switch (type)
        {
            case GL_DEBUG_TYPE_ERROR: return "ERROR";
            case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR: return "DEPRECATED_BEHAVIOR";
            case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR: return "UNDEFINED_BEHAVIOR";
            case GL_DEBUG_TYPE_PORTABILITY: return "PORTABILITY";
            case GL_DEBUG_TYPE_PERFORMANCE: return "PERFORMANCE";
            case GL_DEBUG_TYPE_MARKER: return "MARKER";
            case GL_DEBUG_TYPE_OTHER: return "OTHER";
            default: return "";
        }
    }();

    const auto* const severity_str = [severity]() {
        switch (severity) {
            case GL_DEBUG_SEVERITY_NOTIFICATION: return "NOTIFICATION";
            case GL_DEBUG_SEVERITY_LOW: return "LOW";
            case GL_DEBUG_SEVERITY_MEDIUM: return "MEDIUM";
            case GL_DEBUG_SEVERITY_HIGH: return "HIGH";
            default: return "";
        }
    }();
    std::cout << src_str << ", " << type_str << ", " << severity_str << ", " << id << ": " << message << '\n';
}
}

WindowHandlerSystem::WindowHandlerSystem(entt::registry& registry, entt::entity mainWindow) : _registry{registry} {

    _window = &_registry.emplace<Window>(mainWindow, registry);
    _keyPressedEvent = &_registry.emplace<KeyPressedEventComponent>(mainWindow);
}

void WindowHandlerSystem::InitializeWindow(const int width, const int height, const std::string& windowName) {
    if (!glfwInit())
        std::exit(EXIT_FAILURE);

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    assert(_window);

    _window->Height = height;
    _window->Width = width;

    _window->GlfwWindow = glfwCreateWindow(width, height, windowName.c_str(), nullptr, nullptr);
    if (!_window->GlfwWindow){
        std::cout << "Failed to create the windowComponent" << std::endl;
        glfwTerminate();
    }

    glfwMakeContextCurrent(_window->GlfwWindow);

    if (const int version{gladLoadGL(glfwGetProcAddress)}; version == 0){
        std::cout << "Failed to initialize GLAD" << std::endl;
        glfwTerminate();
    }

    glEnable(GL_DEBUG_OUTPUT);
    glDebugMessageCallback(WindowSystemHelpers::message_callback, nullptr);
    glEnable(GL_DEPTH_TEST);

    // Sets the windowComponent user pointer to this so that in the glfw callbacks the pointer to this can be taken
    glfwSetWindowUserPointer(_window->GlfwWindow, _window);

    // Sets mouse callback
    glfwSetInputMode(_window->GlfwWindow, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    _setFramebufferSizeCallback();
    _setKeyPressedCallback();
    _setMouseMovedCallback();

    // Sets the windowComponent close event
    _keyPressedEvent->OnKeyPressed = [](GLFWwindow* window, int, int, int, int){
        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
            glfwSetWindowShouldClose(window, true);
    };
}

bool WindowHandlerSystem::ShouldWindowClose() const {
    return glfwWindowShouldClose(_window->GlfwWindow);
}

void WindowHandlerSystem::Update() const {
    glfwSwapBuffers(_window->GlfwWindow);
    glfwPollEvents();
}

void WindowHandlerSystem::Clear() {
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void WindowHandlerSystem::_setFramebufferSizeCallback() const {
    auto framebufferSizeCallback = [](GLFWwindow* window, int width, int height) {
        glViewport(0, 0, width, height);

        auto* const selfWindow{static_cast<Window*>(glfwGetWindowUserPointer(window))};

        auto view{selfWindow->Registry.view<FramebufferSizeEventComponent>()};

        view.each([window, width, height](const FramebufferSizeEventComponent &event) {
            if(const auto onFramebufferSizeChanged{event.OnFramebufferSizeChanged}; onFramebufferSizeChanged) {
                onFramebufferSizeChanged(window, width, height);
            }
        });
    };

    glfwSetFramebufferSizeCallback(_window->GlfwWindow, framebufferSizeCallback);
}

void WindowHandlerSystem::_setKeyPressedCallback() const {
    auto keyCallback = [](GLFWwindow* window, int key, int scancode, int action, int mods) {
        auto* const selfWindow{static_cast<Window*>(glfwGetWindowUserPointer(window))};

        auto view{selfWindow->Registry.view<KeyPressedEventComponent>()};

        view.each([window, key, scancode, action, mods](const KeyPressedEventComponent& event) {
            if(const auto onKeyPressed{event.OnKeyPressed}; onKeyPressed) {
                onKeyPressed(window, key, scancode, action, mods);
            }
        });
    };

    glfwSetKeyCallback(_window->GlfwWindow, keyCallback);
}

void WindowHandlerSystem::_setMouseMovedCallback() const {
    auto mouseCallback = [](GLFWwindow* window, double xPos, double yPos) {
        auto* const selfWindow{static_cast<Window*>(glfwGetWindowUserPointer(window))};

        auto view{selfWindow->Registry.view<MouseMovedEventComponent>()};

        view.each([window, xPos, yPos](const MouseMovedEventComponent &event) {
            if(const auto onMouseMoved{event.OnMouseMoved}; onMouseMoved) {
                onMouseMoved(window, xPos, yPos);
            }
        });
    };

    glfwSetCursorPosCallback(_window->GlfwWindow, mouseCallback);
}
