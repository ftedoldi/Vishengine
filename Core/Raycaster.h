#pragma once

#include "DataStructures/Box.h"

#include "entt/entity/entity.hpp"
#include "glm/vec3.hpp"

#include <limits>

struct Ray {
    glm::vec3 Origin{};
    glm::vec3 Direction{0.f, 0.f, -1.f};
    float MaxDistance{std::numeric_limits<float>::max()};
};

struct RaycastHit {
    entt::entity Entity{entt::null};
    float Distance{};
    glm::vec3 Point{};
};

class Raycaster {
public:
    [[nodiscard]] static bool IntersectBox(const Ray& ray, const Box& box, float& distance);
};