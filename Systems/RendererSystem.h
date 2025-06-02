#pragma once

#include "Components/Camera/Camera.h"
#include "Components/Mesh.h"
#include "Material/MaterialController.h"
#include "Math/Transform.h"
#include "Mesh/MeshController.h"
#include "Shaders/Shader.h"

#include <entt/entt.hpp>

#include <memory>

class RendererSystem {
public:
    explicit RendererSystem(Shader* shader);

    void Update(float deltaTime, entt::registry& registry, const MaterialController& materialController, const MeshController& meshController);

private:
    void _bindTextures(const std::vector<Texture>& diffuseTextures, const std::vector<Texture>& specularTextures, const std::vector<Texture>& normalTextures);

    void _drawMeshes(const Transform& cameraTransform, entt::registry& registry);

    void _drawMesh(const Transform& meshViewTransform, uint32_t meshVao, const std::vector<uint32_t>& indices);

    void _drawLights(const Transform& cameraTransform, entt::registry& registry);

    void _drawPointLights(const Transform& cameraTransform, entt::registry& registry);

    void _drawDirectionalLights(const Transform& cameraTransform, entt::registry& registry);

    Transform _calculateWorldTransform(entt::entity parent, entt::registry& registry, const Transform& transform);

    void _setUniformColors(const glm::vec4& colorDiffuse, const glm::vec3& colorSpecular);

    Shader* _shader{};
};