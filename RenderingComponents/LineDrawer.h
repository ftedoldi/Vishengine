#pragma once

#include "Shaders/Shader.h"
#include "entt/entity/registry.hpp"
#include "glad/gl.h"
#include "glm/glm.hpp"

class LineDrawer {
public:
    LineDrawer(const LineDrawer&) = delete;
    LineDrawer(LineDrawer&&) = delete;
    LineDrawer& operator=(const LineDrawer&) = delete;
    LineDrawer& operator=(LineDrawer&&) = delete;

    static void Initialize(entt::registry& registry);
    static LineDrawer& Get();

    ~LineDrawer();

    void DrawLine(glm::vec3 startPoint, glm::vec3 endPoint) const;

    void DrawSphere(glm::vec3 worldSpaceCenter, float radius, uint32_t segments) const;

private:
    LineDrawer(entt::registry& registry);

    static std::unique_ptr<LineDrawer> _lineDrawer;

    std::unique_ptr<Shader> _shader{};

    entt::registry& _registry;

    GLuint _vao{};

    GLuint _vbo{};
};