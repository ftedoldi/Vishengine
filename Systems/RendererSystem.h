#pragma once

#include "Components/Camera/Camera.h"
#include "Components/Mesh.h"
#include "Material/MaterialController.h"
#include "Math/Transform.h"
#include "Mesh/MeshController.h"
#include "Shaders/Shader.h"

#include <entt/entt.hpp>

class RendererSystem {
public:
    explicit RendererSystem(Shader* shader);

    void Update(float deltaTime, entt::registry& registry, const MaterialController& materialController, const MeshController& meshController) const;

private:
    void _bindTextures(const std::vector<Texture>& diffuseTextures, const std::vector<Texture>& specularTextures, const std::vector<Texture>& normalTextures) const;

    void _drawMesh(const std::vector<Transform>& instanceTransforms, const MeshGpuData& gpuData, const std::vector<uint32_t>& indices) const;

    void _drawLights(const Transform& cameraTransform, entt::registry& registry) const;

    void _drawPointLights(const Transform& cameraTransform, entt::registry& registry) const;

    void _drawDirectionalLights(const Transform& cameraTransform, entt::registry& registry) const;

    void _setUniformColors(const glm::vec4& colorDiffuse, const glm::vec3& colorSpecular) const;

    Shader* _shader{};
};