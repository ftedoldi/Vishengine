#pragma once

#include "Controllers/FramebuffersController.h"
#include "Controllers/MaterialController.h"
#include "Controllers/MeshController.h"
#include "Controllers/ShadersController.h"
#include "Coordinates/Transform.h"
#include "Events/WindowEvents.h"
#include "IRenderPass.h"

#include <entt/entt.hpp>

#include <memory>
#include <unordered_map>
#include <vector>

class Shader;

class RendererSystem {
public:
    RendererSystem(entt::dispatcher& windowDispatcher,
                   MaterialController* materialController,
                   MeshController* meshController,
                   ShadersController* shadersController,
                   FramebuffersController* framebuffersController);

    void AddPass(std::unique_ptr<IRenderPass> pass);

    void Update(entt::registry& registry) const;

private:
    void _onFramebufferSizeChanged(WindowsEvents::FrameBufferSizeChangedEvent frameBufferSizeChangedEvent) const;

    void _drawSceneMeshes(entt::entity viewEntity,
                        entt::registry& registry,
                        const std::unordered_map<uint32_t, std::vector<Transform>>& transformsByMeshID) const;

    void _drawDebugFrustumIntersections(entt::registry& registry, entt::entity viewEntity) const;

    void _bindTextures(const Shader* shader,
                       const std::vector<Texture>& diffuseTextures,
                       const std::vector<Texture>& specularTextures) const;

    void _setupLighting(const Shader* shader,
                        const Transform& cameraWorldTransform,
                        entt::registry& registry) const;

    std::vector<std::unique_ptr<IRenderPass>> _passes{};

    MaterialController* _materialController{};

    MeshController* _meshController{};

    ShadersController* _shadersController{};

    FramebuffersController* _framebuffersController{};
};