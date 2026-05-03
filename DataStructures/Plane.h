#pragma once

#include "glm/glm.hpp"

struct Plane {
  // The normal of the plane.
  glm::vec3 Normal{};

  // The distance from the center of the world.
  float Distance{};

};