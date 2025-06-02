#pragma once

#include "glm/detail/type_quat.hpp"
#include "glm/glm.hpp"

class Transform {
public:
    explicit Transform(glm::vec3 position, glm::quat rotation = {0.f, 0.f, 0.f, 1.f}, float scale = 1.f);

    Transform Cumulate(const Transform& otherTransform) const;

    Transform Invert() const;

    glm::vec3 Position{};
    glm::quat Rotation{};
    float Scale{};
};
