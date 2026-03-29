#pragma once

#include "Core/InputManager.h"
#include "DataStructures/Octree.h"
#include "IRenderPass.h"
#include "Platform/Framebuffer.h"
#include "Shaders/Shader.h"

#include <entt/entt.hpp>
#include <glad/gl.h>
#include <memory>

/**
 * Debug render pass that draws bounding spheres as wireframe circles using GL_LINES.
 * Three circles are drawn per sphere (one on each principal plane: XY, XZ, YZ)
 * so the sphere is clearly visible from any angle.
 *
 * The pass renders into the same framebuffer as the scene so the debug
 * geometry is composited on top of the scene image.
 */
class DebugRenderPass : public IRenderPass {
public:
    /**
     * @param inputManager The input manager.
     * @param shader A compiled shader that accepts a "Perspective" mat4 uniform and a "DebugColor" vec3 uniform.
     * @param registry The ECS registry.
     * @param segments Number of line segments used to approximate each circle (default 64).
     */
    DebugRenderPass(Octree::Node* octreeRootNode,
                    InputManager* inputManager,
                    std::unique_ptr<Shader> shader,
                    entt::registry& registry,
                    int segments = 64);

    ~DebugRenderPass() override;

    void Execute() override;

private:
    void _drawBoundingSpheres() const;

    void _drawOctree();

    Octree::Node* _octreeRootNode{};

    InputManager* _inputManager{};

    std::unique_ptr<Shader> _shader{};

    entt::registry& _registry;

    int _segments{};

    // GPU resources for the line geometry (re-used every frame)
    GLuint _vao{};

    GLuint _vbo{};
};
