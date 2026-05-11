#pragma once

#include "Components/RenderingComponents.h"
#include "Controllers/FramebuffersController.h"
#include "Controllers/MaterialController.h"
#include "Controllers/MeshController.h"
#include "Controllers/ShadersController.h"
#include "Coordinates/Transform.h"
#include "Events/WindowEvents.h"
#include "Shaders/Shader.h"

#include <entt/entt.hpp>

#include <memory>
#include <vector>

class RendererSystem {
public:
    RendererSystem(entt::dispatcher& windowDispatcher,
                   MaterialController* materialController,
                   MeshController* meshController,
                   ShadersController* shadersController,
                   FramebuffersController* framebuffersController);

    void Update(entt::registry& registry);

private:
    void _drawSceneMeshes(entt::entity viewEntity,
                          ShaderID shaderId,
                          entt::registry& registry);

    void _drawDebugFrustumIntersections(entt::entity viewEntity, ShaderID shaderId, entt::registry& registry);

    void _bindTextures(const Shader* shader,
                       const std::vector<std::shared_ptr<Texture>>& diffuseTextures,
                       const std::vector<std::shared_ptr<Texture>>& specularTextures);

    void _setupLighting(const Shader* shader,
                        const Transform& cameraViewTransform,
                        entt::registry& registry);
    void _onFramebufferSizeChanged(WindowsEvents::FrameBufferSizeChangedEvent frameBufferSizeChangedEvent) const;

    MaterialController* _materialController{};

    MeshController* _meshController{};

    ShadersController* _shadersController{};

    FramebuffersController* _framebuffersController{};

    uint32_t _globalInstanceSsbo{};

    size_t _ssboCapacityBytes{};
};