#include "LineDrawer.h"

#include "Components/Camera/EditorCameraTag.h"

#include <glm/gtc/constants.hpp>

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

        out.push_back(center + radius * (glm::cos(a0) * axis0 + glm::sin(a0) * axis1));
        out.push_back(center + radius * (glm::cos(a1) * axis0 + glm::sin(a1) * axis1));
    }
}

void AppendBoxEdges(const glm::vec3& min,
                    const glm::vec3& max,
                    std::vector<glm::vec3>& out) {
    const glm::vec3 corners[8] = {
        {min.x, min.y, min.z}, {max.x, min.y, min.z},
        {max.x, max.y, min.z}, {min.x, max.y, min.z},
        {min.x, min.y, max.z}, {max.x, min.y, max.z},
        {max.x, max.y, max.z}, {min.x, max.y, max.z},
    };

    constexpr int edges[12][2] = {
        {0, 1}, {1, 2}, {2, 3}, {3, 0},   // bottom
        {4, 5}, {5, 6}, {6, 7}, {7, 4},   // top
        {0, 4}, {1, 5}, {2, 6}, {3, 7},   // verticals
    };

    for (const auto& edge : edges) {
        out.push_back(corners[edge[0]]);
        out.push_back(corners[edge[1]]);
    }
}

} // namespace

LineDrawer::Batch::Batch(const LineDrawer* owner,
                         const Camera& camera,
                         const glm::vec3& color)
    : _owner{owner}, _camera{camera}, _color{color} {
    // Reasonable default reservation; AddBox/AddSphere will grow as needed.
    _vertices.reserve(256);
}

LineDrawer::Batch::~Batch() {
    if (!_flushed) {
        Flush();
    }
}

void LineDrawer::Batch::AddLine(const glm::vec3& startWorld, const glm::vec3& endWorld) {
    _vertices.push_back(_camera.ViewTransform.TransformPosition(startWorld));
    _vertices.push_back(_camera.ViewTransform.TransformPosition(endWorld));
}

void LineDrawer::Batch::AddSphere(const glm::vec3& centerWorld,
                                  const float radius,
                                  const uint32_t segments) {
    const glm::vec3 centerView{_camera.ViewTransform.TransformPosition(centerWorld)};
    const int seg{static_cast<int>(segments)};

    _vertices.reserve(_vertices.size() + static_cast<size_t>(segments) * 6);

    AppendCircle(centerView, radius, {1.f, 0.f, 0.f}, {0.f, 1.f, 0.f}, seg, _vertices);
    AppendCircle(centerView, radius, {1.f, 0.f, 0.f}, {0.f, 0.f, 1.f}, seg, _vertices);
    AppendCircle(centerView, radius, {0.f, 1.f, 0.f}, {0.f, 0.f, 1.f}, seg, _vertices);
}

void LineDrawer::Batch::AddBox(const glm::vec3& minWorld, const glm::vec3& maxWorld) {
    const size_t startIndex{_vertices.size()};
    AppendBoxEdges(minWorld, maxWorld, _vertices);

    // Transform the 24 vertices we just appended into view space.
    for (size_t i = startIndex; i < _vertices.size(); ++i) {
        _vertices[i] = _camera.ViewTransform.TransformPosition(_vertices[i]);
    }
}

void LineDrawer::Batch::Flush() {
    assert(_owner);

    if (_flushed) {
        return;
    }

    _flushed = true;

    if (!_vertices.empty()) {
        _owner->_submit(_camera, _color, _vertices);
    }
    _vertices.clear();
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
    _shader = std::make_unique<Shader>(shadersBasePath + "debug_vertex.glsl",
                                       shadersBasePath + "debug_fragment.glsl");

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

const Camera* LineDrawer::_getEditorCamera() const {
    const auto view{_registry.view<Camera, EditorCameraTag>()};
    for (const auto& [entity, camera] : view.each()) {
        return &camera;
    }
    return nullptr;
}

void LineDrawer::_submit(const Camera& camera,
                         const glm::vec3& color,
                         const std::vector<glm::vec3>& viewSpaceVertices) const {
    _shader->UseProgram();
    _shader->SetUniformMat4("Perspective", camera.ProjectionMatrix);
    _shader->SetUniformVec3("DebugColor", color.r, color.g, color.b);

    glBindVertexArray(_vao);
    glBindBuffer(GL_ARRAY_BUFFER, _vbo);
    glBufferData(GL_ARRAY_BUFFER,
                 static_cast<GLsizeiptr>(viewSpaceVertices.size() * sizeof(glm::vec3)),
                 viewSpaceVertices.data(),
                 GL_DYNAMIC_DRAW);

    glDrawArrays(GL_LINES, 0, static_cast<GLsizei>(viewSpaceVertices.size()));
    glBindVertexArray(0);
}

LineDrawer::Batch LineDrawer::BeginBatch(const Camera& camera, const glm::vec3 color) const {
    // TODO: I should add the possibility to add more cameras
    return Batch{this, camera, color};
}

void LineDrawer::DrawLine(const glm::vec3 startWorld,
                          const glm::vec3 endWorld,
                          const glm::vec3 color) const {
    auto batch{BeginBatch(*_getEditorCamera(), color)};
    batch.AddLine(startWorld, endWorld);
}

void LineDrawer::DrawSphere(const glm::vec3 centerWorld,
                            const float radius,
                            const uint32_t segments,
                            const glm::vec3 color) const {
    auto batch{BeginBatch(*_getEditorCamera(), color)};
    batch.AddSphere(centerWorld, radius, segments);
}

void LineDrawer::DrawBox(const glm::vec3 minWorld,
                         const glm::vec3 maxWorld,
                         const glm::vec3 color) const {
    auto batch{BeginBatch(*_getEditorCamera(), color)};
    batch.AddBox(minWorld, maxWorld);
}