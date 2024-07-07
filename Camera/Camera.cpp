#include "Camera.h"

Camera::Camera(const glm::vec3 position, const glm::vec3 up, const glm::vec3 front) :
    Position(position), Up(up), Front(front) {

    PerspectiveMatrix = glm::perspective(glm::radians(45.f), static_cast<float>(800) / static_cast<float>(600), 0.1f, 100.f);
}