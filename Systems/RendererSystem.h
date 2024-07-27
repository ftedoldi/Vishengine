#pragma once

#include "Components/CameraComponents/Camera.h"
#include "Components/Mesh.h"
#include "Components/TextureList.h"
#include "Shaders/Shader.h"

#include <entt/entt.hpp>

#include <memory>

namespace RendererUtils {
struct Transform {
    Transform(glm::vec3 position, glm::quat rotation, float scale) : Position{position}, Rotation{rotation}, Scale{scale} {}

    glm::vec3 Position{};
    glm::quat Rotation{};
    float Scale{1.f};
};
}

class RendererSystem {
public:
    explicit RendererSystem(entt::registry& registry, Shader* shader, entt::entity currentCamera);

    void SetCurrentCamera(entt::entity camera);

    void Update(float deltaTime);

private:
    void _bindTextures(const Mesh& mesh);
    void _drawMeshes();
    void _drawMesh(const Mesh& mesh, const RendererUtils::Transform& meshTransform);

    void _drawLights();

    void _setUniformColors(const Mesh& mesh);

    entt::entity _currentCameraToRender{};
    entt::registry& _registry;
    Shader* _shader;
};