#pragma once

#include "IRenderPass.h"

#include "Material/MaterialController.h"
#include "Math/Transform.h"
#include "Mesh/MeshController.h"
#include "Shaders/Shader.h"

#include <entt/entt.hpp>

class SceneRenderPass : public IRenderPass {
public:
    SceneRenderPass(std::unique_ptr<Shader> shader,
        entt::registry& registry,
        const std::shared_ptr<MaterialController>& materialController,
        const std::shared_ptr<MeshController>& meshController);

    ~SceneRenderPass() override;

    void Execute() override;

private:
    void _render() const;

    void _bindTextures(const std::vector<Texture>& diffuseTextures,
                       const std::vector<Texture>& specularTextures,
                       const std::vector<Texture>& normalTextures) const;

    void _drawLights(const Transform& cameraTransform, entt::registry& registry) const;

    void _drawPointLights(const Transform& cameraTransform, entt::registry& registry) const;

    void _drawDirectionalLights(const Transform& cameraTransform, entt::registry& registry) const;

    void _setUniformColors(const glm::vec4& colorDiffuse, const glm::vec3& colorSpecular) const;

    std::unique_ptr<Shader> _shader{};

    entt::registry& _registry;

    std::shared_ptr<MaterialController> _materialController{};

    std::shared_ptr<MeshController> _meshController{};
};
