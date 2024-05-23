#include "Window.h"

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

static void TerminateProgram() {
    glfwTerminate();
    std::exit(1);
}

Window::Window() {
    _initializeWindow();
}

Window::Window(const int width, const int height, const std::string& windowName) {
    _initializeWindow(width, height, windowName);
}

Window::~Window() {
    glfwDestroyWindow(_window);
}

bool Window::ShouldWindowClose() const {
    return glfwWindowShouldClose(_window);
}

void Window::Update() const {
    glfwSwapBuffers(_window);
    glfwPollEvents();
}

int Window::GetWidth() const {
    return _width;
}

int Window::GetHeight() const {
    return _height;
}

void Window::Clear() {
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
}

KeyboardKeyPressed& Window::OnKeyboardKeyPressed() {
    return _onKeyboardKeyPressed;
}

FramebufferSizeChanged& Window::OnFramebufferSizeChanged() {
    return _onFramebufferSizeChanged;
}

InputManager Window::CreateInputManager() {
    return InputManager{_window};
}

void Window::_initializeWindow(const int width, const int height, const std::string &windowName) {
    if (!glfwInit())
        std::exit(EXIT_FAILURE);

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    _width = width;
    _height = height;

    _window = glfwCreateWindow(width, height, windowName.c_str(), nullptr, nullptr);
    if (!_window){
        std::cout << "Failed to create the window" << std::endl;
        TerminateProgram();
    }

    glfwMakeContextCurrent(_window);

    if (const int version{gladLoadGL(glfwGetProcAddress)}; version == 0){
        std::cout << "Failed to initialize GLAD" << std::endl;
        TerminateProgram();
    }

    glEnable(GL_DEBUG_OUTPUT);
    glDebugMessageCallback(message_callback, nullptr);

    // Sets the window user pointer to this so that in the glfw callbacks the pointer to this can be taken
    glfwSetWindowUserPointer(_window, this);

    _setFramebufferSizeCallback();
    _setKeyPressedCallback();

    // Sets the window close event
    _onKeyboardKeyPressed.AddFunction([this](GLFWwindow*, int, int){
        if (glfwGetKey(_window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
            glfwSetWindowShouldClose(_window, true);
    });
}

void Window::_setFramebufferSizeCallback() const {
    auto framebufferSizeCallback = [](GLFWwindow* window, int width, int height) {
        glViewport(0, 0, width, height);

        auto *const selfWindow{static_cast<Window*>(glfwGetWindowUserPointer(window))};
        selfWindow->_onFramebufferSizeChanged.Broadcast(width, height);
    };

    glfwSetFramebufferSizeCallback(_window, framebufferSizeCallback);
}

void Window::_setKeyPressedCallback() const {
    auto keyCallback = [](GLFWwindow* window, int key, int, int action, int) {
        auto *const selfWindow{static_cast<Window*>(glfwGetWindowUserPointer(window))};
        selfWindow->_onKeyboardKeyPressed.Broadcast(window, key, action);
    };

    glfwSetKeyCallback(_window, keyCallback);
}