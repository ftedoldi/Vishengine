#pragma once

#include "Components/CameraComponents/Camera.h"
#include "Components/Mesh.h"
#include "Components/TextureList.h"
#include "Shaders/Shader.h"

#include <entt/entt.hpp>

#include <memory>

namespace RendererUtils {
struct Transform {
    explicit Transform(glm::vec3 position, glm::quat rotation = {0.f, 0.f, 0.f, 1.f}, float scale = 1.f) : Position{position}, Rotation{rotation}, Scale{scale} {}

    glm::vec3 Position{};
    glm::quat Rotation{};
    float Scale{};
};
}

class RendererSystem {
public:
    explicit RendererSystem(entt::registry& registry, Shader* shader, entt::entity currentCamera);

    void SetCurrentCamera(entt::entity camera);

    void Update(float deltaTime);

private:
    void _bindTextures(const Mesh& mesh);
    void _drawMeshes(const RendererUtils::Transform& cameraTransform);
    void _drawMesh(const Mesh& mesh, const RendererUtils::Transform& meshTransform, const RendererUtils::Transform& cameraTransform);

    void _drawLights(const RendererUtils::Transform& cameraTransform);

    RendererUtils::Transform _calculateWorldTransform(entt::entity parent, const RendererUtils::Transform& transform);

    void _setUniformColors(const Mesh& mesh);

    entt::entity _currentCameraToRender{};
    entt::registry& _registry;
    Shader* _shader;
};