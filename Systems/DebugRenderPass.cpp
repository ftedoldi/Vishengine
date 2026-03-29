#include "DebugRenderPass.h"

#include "Components/BoundingSphere.h"
#include "Components/Camera/Camera.h"
#include "Components/Camera/EditorCameraTag.h"
#include "Components/Transforms/WorldTransform.h"

#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

#include <vector>

// Helper: append line-segment vertices for one circle
//
//   center   – centre of the circle in view space
//   radius   – radius of the circle
//   axis0/1  – two orthogonal unit vectors that span the circle's plane
//   segments – number of segments (= number of vertices pairs)
//   out      – destination buffer
namespace {

    void AppendCircle(const glm::vec3& center,
                         const float radius,
                         const glm::vec3& axis0,
                         const glm::vec3& axis1,
                         const int segments,
                         std::vector<glm::vec3>& out) {
        const float step{glm::two_pi<float>() / static_cast<float>(segments)};
        for (int i = 0; i < segments; ++i) {
            const float a0{step * static_cast<float>(i)};
            const float a1{step * static_cast<float>(i + 1)};

            const glm::vec3 p0{center + radius * (glm::cos(a0) * axis0 + glm::sin(a0) * axis1)};
            const glm::vec3 p1{center + radius * (glm::cos(a1) * axis0 + glm::sin(a1) * axis1)};

            out.push_back(p0);
            out.push_back(p1);
        }
    }

    // Append the 12 edges (24 vertices) of an axis-aligned box defined by min/max corners.
    void AppendBox(const glm::vec3& min, const glm::vec3& max, std::vector<glm::vec3>& out) {
        // 8 corners of the box
        const glm::vec3 corners[8] = {
            {min.x, min.y, min.z},
            {max.x, min.y, min.z},
            {max.x, max.y, min.z},
            {min.x, max.y, min.z},
            {min.x, min.y, max.z},
            {max.x, min.y, max.z},
            {max.x, max.y, max.z},
            {min.x, max.y, max.z},
        };

        // 12 edges as index pairs
        constexpr int edges[12][2] = {
            {0, 1}, {1, 2}, {2, 3}, {3, 0}, // bottom face
            {4, 5}, {5, 6}, {6, 7}, {7, 4}, // top face
            {0, 4}, {1, 5}, {2, 6}, {3, 7}, // vertical edges
        };

        for (const auto& edge : edges) {
            out.push_back(corners[edge[0]]);
            out.push_back(corners[edge[1]]);
        }
    }

    // Recursively collect box line vertices for all octree nodes.
    void CollectOctreeBoxes(const Octree::Node* node,
                            const Transform& viewTransform,
                            std::vector<glm::vec3>& out) {
        if (!node) {
            return;
        }

        const glm::vec3 worldMin{node->Center - glm::vec3{node->HalfWidth}};
        const glm::vec3 worldMax{node->Center + glm::vec3{node->HalfWidth}};

        // Transform corners to view space
        const glm::vec3 viewMin{viewTransform.TransformPosition(worldMin)};
        const glm::vec3 viewMax{viewTransform.TransformPosition(worldMax)};

        AppendBox(viewMin, viewMax, out);

        for (const auto& child : node->Children) {
            CollectOctreeBoxes(child.get(), viewTransform, out);
        }
    }

}

DebugRenderPass::DebugRenderPass(Octree::Node* const  octreeRootNode,
                                 InputManager* const inputManager,
                                 std::unique_ptr<Shader> shader,
                                 entt::registry& registry,
                                 const int segments)
    : _octreeRootNode{octreeRootNode}
    , _inputManager{inputManager}
    , _shader{std::move(shader)}
    , _registry{registry}
    , _segments{segments} {
    glGenVertexArrays(1, &_vao);
    glGenBuffers(1, &_vbo);

    glBindVertexArray(_vao);
    glBindBuffer(GL_ARRAY_BUFFER, _vbo);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), nullptr);

    glBindVertexArray(0);
}

DebugRenderPass::~DebugRenderPass() {
    glDeleteBuffers(1, &_vbo);
    glDeleteVertexArrays(1, &_vao);
    _shader->DeleteProgram();
}

void DebugRenderPass::Execute() {
    if (_inputManager->IsKeyPressed(GLFW_KEY_B)) {
        _drawBoundingSpheres();
    }

    if (_inputManager->IsKeyPressed(GLFW_KEY_O)) {
        _drawOctree();
    }
}

void DebugRenderPass::_drawBoundingSpheres() const {
    // Collect the active editor camera
    const auto cameraView{_registry.view<Camera, WorldTransform, EditorCameraTag>()};

    for (const auto& [cameraEntity, camera, cameraWorldTransform] : cameraView.each()) {
        _shader->UseProgram();
        _shader->SetUniformMat4("Perspective", camera.ProjectionMatrix);
        // Draw bounding spheres in a bright green so they stand out
        _shader->SetUniformVec3("DebugColor", 0.f, 1.f, 0.f);

        // Build line vertices for every entity that has a BoundingSphere
        std::vector<glm::vec3> lineVertices{};
        lineVertices.reserve(static_cast<size_t>(_segments) * 6); // 3 circles × 2 verts/segment

        const auto sphereView{_registry.view<BoundingSphere>()};
        for (const auto& [entity, sphere] : sphereView.each()) {
            // Transform the world-space sphere center into view space
            const auto centerViewSpace{camera.ViewTransform.TransformPosition(sphere.WorldCenter)};

            // XY plane  (normal = Z)
            AppendCircle(centerViewSpace, sphere.WorldRadius, {1.f, 0.f, 0.f}, {0.f, 1.f, 0.f}, _segments, lineVertices);

            // XZ plane  (normal = Y)
            AppendCircle(centerViewSpace, sphere.WorldRadius, {1.f, 0.f, 0.f}, {0.f, 0.f, 1.f}, _segments, lineVertices);

            // YZ plane  (normal = X)
            AppendCircle(centerViewSpace, sphere.WorldRadius, {0.f, 1.f, 0.f}, {0.f, 0.f, 1.f}, _segments, lineVertices);
        }

        if (lineVertices.empty()) {
            return;
        }

        // Upload and draw
        glBindVertexArray(_vao);
        glBindBuffer(GL_ARRAY_BUFFER, _vbo);
        glBufferData(GL_ARRAY_BUFFER,
                     static_cast<GLsizeiptr>(lineVertices.size() * sizeof(glm::vec3)),
                     lineVertices.data(),
                     GL_DYNAMIC_DRAW);

        glDrawArrays(GL_LINES, 0, static_cast<GLsizei>(lineVertices.size()));
        glBindVertexArray(0);
    }
}

void DebugRenderPass::_drawOctree() {
    assert(_octreeRootNode);

    const auto cameraView{_registry.view<Camera, WorldTransform, EditorCameraTag>()};

    for (const auto& [cameraEntity, camera, cameraWorldTransform] : cameraView.each()) {
        _shader->UseProgram();
        _shader->SetUniformMat4("Perspective", camera.ProjectionMatrix);
        // Draw octree node boxes in yellow so they are distinct from bounding spheres
        _shader->SetUniformVec3("DebugColor", 1.f, 1.f, 0.f);

        // Each node contributes 12 edges × 2 vertices = 24 vertices.
        // Reserve a reasonable initial capacity; the vector will grow as needed.
        std::vector<glm::vec3> lineVertices{};
        lineVertices.reserve(24 * 64);

        CollectOctreeBoxes(_octreeRootNode, camera.ViewTransform, lineVertices);

        if (lineVertices.empty()) {
            return;
        }

        // Upload and draw
        glBindVertexArray(_vao);
        glBindBuffer(GL_ARRAY_BUFFER, _vbo);
        glBufferData(GL_ARRAY_BUFFER,
                     static_cast<GLsizeiptr>(lineVertices.size() * sizeof(glm::vec3)),
                     lineVertices.data(),
                     GL_DYNAMIC_DRAW);

        glDrawArrays(GL_LINES, 0, static_cast<GLsizei>(lineVertices.size()));
        glBindVertexArray(0);
    }
}
