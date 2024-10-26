#pragma once

#include "Components/CameraComponents/Camera.h"
#include "Components/Mesh.h"
#include "Components/TextureList.h"
#include "Shaders/Shader.h"
#include "Core/Transform.h"

#include <entt/entt.hpp>

#include <memory>

class RendererSystem {
public:
    explicit RendererSystem(entt::registry& registry, Shader* shader, entt::entity currentCamera);

    void SetCurrentCamera(entt::entity camera);

    void Update(float deltaTime);

private:
    void _bindTextures(const Mesh& mesh);
    void _drawMeshes(const Transform& cameraTransform);
    void _drawMesh(const Mesh& mesh, const Transform& meshTransform, const Transform& cameraTransform);

    void _drawLights(const Transform& cameraTransform);

    void _drawPointLights(const Transform& cameraTransform);

    void _drawDirectionalLights(const Transform& cameraTransform);

    Transform _calculateWorldTransform(entt::entity parent, const Transform& transform);

    Transform _calculateViewTransform(const Transform& transform, const Transform& cameraTransform);

    void _setUniformColors(const Mesh& mesh);

    entt::entity _currentCameraToRender{};
    entt::registry& _registry;
    Shader* _shader;
};