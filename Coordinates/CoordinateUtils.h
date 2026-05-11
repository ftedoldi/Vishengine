#pragma once

#include "DataStructures/Box.h"

#include "entt/entt.hpp"
#include "glm/fwd.hpp"

namespace CoordUtils {

    Box ComputeWorldSpaceBox(entt::entity entity, entt::registry& registry);

    glm::vec3 RotateVectorByQuaternion(glm::quat quaternion, glm::vec3 vector);

}