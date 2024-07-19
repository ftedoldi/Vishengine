#include "InputManager.h"

InputManager::InputManager(entt::registry& registry, const entt::entity mainWindow) {
    auto view{registry.view<Window>()};
    _mainWindow = &registry.get<Window>(mainWindow);

}
bool InputManager::IsKeyPressed(const int key) {
    return glfwGetKey(_mainWindow->GlfwWindow, key) == GLFW_PRESS;
}
