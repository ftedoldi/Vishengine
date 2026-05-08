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

#include <vector>

class RendererSystem {
public:
    RendererSystem(entt::dispatcher& windowDispatcher,
                   MaterialController* materialController,
                   MeshController* meshController,
                   ShadersController* shadersController,
                   FramebuffersController* framebuffersController);

    void Update(entt::registry& registry) const;

private:
    struct MeshInstance {
        uint32_t meshID{};
        Transform transform{};
        bool renderable{};
    };

    void _onFramebufferSizeChanged(WindowsEvents::FrameBufferSizeChangedEvent frameBufferSizeChangedEvent) const;

    void _drawSceneMeshes(entt::entity viewEntity,
                        ShaderID shaderId,
                        entt::registry& registry,
                        MeshSet meshSet) const;

    void _drawDebugFrustumIntersections(entt::entity viewEntity, ShaderID shaderId, entt::registry& registry) const;

    void _bindTextures(const Shader* shader,
                       const std::vector<Texture>& diffuseTextures,
                       const std::vector<Texture>& specularTextures) const;

    void _setupLighting(const Shader* shader,
                        const Transform& cameraViewTransform,
                        entt::registry& registry) const;

    mutable std::vector<MeshInstance> _meshInstances;

    MaterialController* _materialController{};

    MeshController* _meshController{};

    ShadersController* _shadersController{};

    FramebuffersController* _framebuffersController{};
};