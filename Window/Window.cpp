#include "Window.h"

#include "../Camera/Camera.h"

#include "glm/ext/matrix_clip_space.hpp"

static void message_callback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei, const GLchar* message, const void*){
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

static void TerminateProgram(){
    glfwTerminate();
    std::exit(1);
}

void Window::InitializeWindow(){
    if (!glfwInit())
        std::exit(EXIT_FAILURE);

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    _window = glfwCreateWindow(_width, _height, _windowName.c_str(), nullptr, nullptr);
    if (!_window){
        std::cout << "Failed to create the window" << std::endl;
        TerminateProgram();
    }

    glfwMakeContextCurrent(_window);

    if (int version{gladLoadGL(glfwGetProcAddress)}; version == 0){
        std::cout << "Failed to initialize GLAD" << std::endl;
        TerminateProgram();
    }

    glEnable(GL_DEBUG_OUTPUT);
    glDebugMessageCallback(message_callback, nullptr);

    _setFramebufferSizeCallback();

    Camera::PerspectiveMatrix = glm::perspective(glm::radians(45.f), static_cast<float>(_width) / static_cast<float>(_height), 0.1f, 100.f);
}

bool Window::ShouldWindowClose(){
    return glfwWindowShouldClose(_window);
}

void Window::Update()
{
    glfwSwapBuffers(_window);
    glfwPollEvents();
}

void Window::_setFramebufferSizeCallback()
{
    auto framebufferSizeCallback = [](GLFWwindow*, int width, int height) {
        glViewport(0, 0, width, height);
        Camera::PerspectiveMatrix = glm::perspective(glm::radians(45.f), static_cast<float>(width) / static_cast<float>(height), 0.1f, 100.f);
    };

    glfwSetFramebufferSizeCallback(_window, framebufferSizeCallback);
}

void Window::Clear() {
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
}

int Window::GetWidth() const {
    return _width;
}

int Window::GetHeight() const {
    return _height;
}