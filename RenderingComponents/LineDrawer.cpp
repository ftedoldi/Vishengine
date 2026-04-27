#include "LineDrawer.h"

#include "Components/Camera/Camera.h"
#include "Components/Camera/EditorCameraTag.h"
#include "glm/gtc/constants.hpp"

std::unique_ptr<LineDrawer> LineDrawer::_lineDrawer{};

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
std::vector<glm::vec3> AppendBox(const glm::vec3& min, const glm::vec3& max) {
    std::vector<glm::vec3> out{};
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

    return out;
}

}

void LineDrawer::Initialize(entt::registry& registry) {
    _lineDrawer = std::unique_ptr<LineDrawer>(new LineDrawer(registry));
}

LineDrawer& LineDrawer::Get() {
    assert(_lineDrawer);
    return *_lineDrawer;
}

LineDrawer::LineDrawer(entt::registry& registry) : _registry{registry} {
    const std::string shadersBasePath{std::string(PROJECT_SOURCE_DIR) + "/Shaders/GlslShaders/"};
    _shader = std::make_unique<Shader>(shadersBasePath + "debug_vertex.glsl", shadersBasePath + "debug_fragment.glsl");

    glGenVertexArrays(1, &_vao);
    glGenBuffers(1, &_vbo);

    glBindVertexArray(_vao);
    glBindBuffer(GL_ARRAY_BUFFER, _vbo);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), nullptr);

    glBindVertexArray(0);
}

LineDrawer::~LineDrawer() {
    glDeleteBuffers(1, &_vbo);
    glDeleteVertexArrays(1, &_vao);
}

void LineDrawer::DrawLine(const glm::vec3 startPoint, const glm::vec3 endPoint) const {
    const auto cameraView{_registry.view<Camera, EditorCameraTag>()};

    for (const auto& [cameraEntity, camera] : cameraView.each()) {
        _shader->UseProgram();
        _shader->SetUniformMat4("Perspective", camera.ProjectionMatrix);
        _shader->SetUniformVec3("DebugColor", 0.f, 1.f, 0.f);

        const glm::vec3 points[2] = {camera.ViewTransform.TransformPosition(startPoint), camera.ViewTransform.TransformPosition(endPoint)};
        glBindVertexArray(_vao);
        glBindBuffer(GL_ARRAY_BUFFER, _vbo);
        glBufferData(GL_ARRAY_BUFFER,
                     static_cast<GLsizeiptr>(2 * sizeof(glm::vec3)),
                     points,
                     GL_DYNAMIC_DRAW);

        glDrawArrays(GL_LINES, 0, 2);
        glBindVertexArray(0);
    }
}

void LineDrawer::DrawSphere(const glm::vec3 worldSpaceCenter, const float radius, const uint32_t segments) const {
    // TODO: fix and also use the line drawer in the debug functions
    const auto cameraView{_registry.view<Camera, EditorCameraTag>()};

    for (const auto& [cameraEntity, camera] : cameraView.each()) {
        const auto centerViewSpace{camera.ViewTransform.TransformPosition(worldSpaceCenter)};

        std::vector<glm::vec3> lineVertices{};
        lineVertices.reserve(static_cast<size_t>(segments) * 6);

        // XY plane  (normal = Z)
        AppendCircle(centerViewSpace, radius, {1.f, 0.f, 0.f}, {0.f, 1.f, 0.f}, segments, lineVertices);

        // XZ plane  (normal = Y)
        AppendCircle(centerViewSpace, radius, {1.f, 0.f, 0.f}, {0.f, 0.f, 1.f}, segments, lineVertices);

        // YZ plane  (normal = X)
        AppendCircle(centerViewSpace, radius, {0.f, 1.f, 0.f}, {0.f, 0.f, 1.f}, segments, lineVertices);

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