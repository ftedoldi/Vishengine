#pragma once

#include "DataStructures/Box.h"

#include "entt/entt.hpp"

namespace CoordUtils {

    Box ComputeWorldSpaceBox(entt::entity entity, entt::registry& registry);

    glm::vec3 RotateVectorByQuaternion(const glm::quat quaternion, const glm::vec3 vector);

}