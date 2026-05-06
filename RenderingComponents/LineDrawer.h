#pragma once

#include "Components/Camera/Camera.h"
#include "Shaders/Shader.h"

#include <entt/entt.hpp>
#include <glm/glm.hpp>

#include <cstdint>
#include <memory>
#include <vector>

class LineDrawer {
public:
    // RAII batch handle. Accumulates geometry in world space and flushes on destruction.
    // Move-only; do not create nested batches.
    class Batch {
    public:
        ~Batch();

        Batch(Batch&&) noexcept = default;
        Batch& operator=(Batch&&) noexcept = default;

        Batch(const Batch&) = delete;
        Batch& operator=(const Batch&) = delete;

        void AddLine(const glm::vec3& startWorld, const glm::vec3& endWorld);

        void AddSphere(const glm::vec3& centerWorld, float radius, uint32_t segments = 16);

        void AddBox(const glm::vec3& minWorld, const glm::vec3& maxWorld);

        // Manually flush before destruction (rarely needed).
        void Flush();

    private:
        friend class LineDrawer;
        Batch(const LineDrawer* owner, const Camera& camera, const glm::vec3& color);

        const LineDrawer* _owner{};

        const Camera& _camera;

        glm::vec3 _color{1.f};

        std::vector<glm::vec3> _vertices{};

        bool _flushed{false};
    };

    static void Initialize(entt::registry& registry);
    static LineDrawer& Get();

    ~LineDrawer();

    LineDrawer(const LineDrawer&) = delete;
    LineDrawer& operator=(const LineDrawer&) = delete;

    // Start a batched draw. Returns an empty/no-op batch if no editor camera exists.
    [[nodiscard]] Batch BeginBatch(const Camera& camera, glm::vec3 color = {0.f, 1.f, 0.f}) const;

    // Convenience immediate-mode wrappers (each creates a one-shot batch).
    void DrawLine(glm::vec3 startWorld, glm::vec3 endWorld, glm::vec3 color = {0.f, 1.f, 0.f}) const;

    void DrawSphere(glm::vec3 centerWorld, float radius, uint32_t segments = 16, glm::vec3 color = {0.f, 1.f, 0.f}) const;

    void DrawBox(glm::vec3 minWorld, glm::vec3 maxWorld, glm::vec3 color = {1.f, 1.f, 0.f}) const;

private:
    explicit LineDrawer(entt::registry& registry);

    const Camera* _getEditorCamera() const;

    void _submit(const Camera& camera, const glm::vec3& color, const std::vector<glm::vec3>& viewSpaceVertices) const;

    static std::unique_ptr<LineDrawer> _lineDrawer;

    entt::registry& _registry;

    std::unique_ptr<Shader> _shader{};

    unsigned int _vao{0};

    unsigned int _vbo{0};
};