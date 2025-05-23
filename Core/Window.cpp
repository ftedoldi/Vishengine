#include "Window.h"

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

Window::~Window() {
    glfwDestroyWindow(_window);
    glfwTerminate();
}

void Window::Initialize(const int width, const int height, const std::string& windowName) {
    if (!glfwInit())
        std::exit(EXIT_FAILURE);

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    _width = width;
    _height = height;

    _window = glfwCreateWindow(width, height, windowName.c_str(), nullptr, nullptr);
    if (!_window){
        std::cout << "Failed to create the windowComponent" << std::endl;
        glfwTerminate();
    }

    glfwMakeContextCurrent(_window);

    if (const int version{gladLoadGL(glfwGetProcAddress)}; version == 0){
        std::cout << "Failed to initialize GLAD" << std::endl;
        glfwTerminate();
    }

    glEnable(GL_DEBUG_OUTPUT);
    glDebugMessageCallback(WindowSystemHelpers::message_callback, nullptr);
    glEnable(GL_DEPTH_TEST);

    // Sets the windowComponent user pointer to this so that in the glfw callbacks the pointer to this can be taken
    glfwSetWindowUserPointer(_window, this);

    // Sets mouse callback
    glfwSetInputMode(_window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);

    _setFramebufferSizeCallback();
    _setKeyPressedCallback();
    _setMouseMovedCallback();

    // Sets the windowComponent close event
    _eventDispatcher.sink<KeyPressedEvent>().connect<&Window::_shouldCloseWindow>(this);
}

bool Window::ShouldWindowClose() const {
    return glfwWindowShouldClose(_window);
}

void Window::Update() const {
    //glfwPollEvents();

    glfwSwapBuffers(_window);
}

void Window::Clear() {
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

double Window::GetHeight() const {
    return _height;
}

double Window::GetWidth() const {
    return _width;
}

void Window::_setFramebufferSizeCallback() const {
    auto framebufferSizeCallback = [](GLFWwindow* window, int width, int height) {
        glViewport(0, 0, width, height);

        auto* const selfWindow{static_cast<Window*>(glfwGetWindowUserPointer(window))};
        if(selfWindow) {
            selfWindow->_eventDispatcher.trigger<FrameBufferSizeChangedEvent>({static_cast<float>(width), static_cast<float>(height)});
        }
    };

    glfwSetFramebufferSizeCallback(_window, framebufferSizeCallback);
}

void Window::_setKeyPressedCallback() const {
    auto keyCallback = [](GLFWwindow* window, int key, int scancode, int action, int mods) {
        auto* const selfWindow{static_cast<Window*>(glfwGetWindowUserPointer(window))};
        if(selfWindow) {
            selfWindow->_eventDispatcher.trigger<KeyPressedEvent>({key, scancode, action, mods});
        }
    };

    glfwSetKeyCallback(_window, keyCallback);
}

void Window::_setMouseMovedCallback() const {
    auto mouseCallback = [](GLFWwindow* window, double xPos, double yPos) {
        auto* const selfWindow{static_cast<Window*>(glfwGetWindowUserPointer(window))};
        if(selfWindow) {
            selfWindow->_eventDispatcher.trigger<MouseMovedEvent>({xPos, yPos});
        }
    };

    glfwSetCursorPosCallback(_window, mouseCallback);
}

entt::dispatcher& Window::GetEventDispatcher() {
    return _eventDispatcher;
}

void Window::_shouldCloseWindow(const KeyPressedEvent& keyPressedEvent) {
    if (keyPressedEvent.Key == GLFW_KEY_ESCAPE) {
        glfwSetWindowShouldClose(_window, true);
    }
}

GLFWwindow* Window::GetGLFWwindow() {
    return _window;
}
