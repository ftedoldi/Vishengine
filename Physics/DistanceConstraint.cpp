#include "DistanceConstraint.h"

DistanceConstraint::DistanceConstraint(glm::vec3& p1, glm::vec3& p2, const float w1, const float w2)
    : _p1{p1},
      _p2{p2},
      _w1{w1},
      _w2{w2},
      _restLength{glm::length(_p1 - _p2)} {
}

void DistanceConstraint::Project() {
    // Formula : deltaP1 = -w1 / (w1 + w2) * (||p1 - p2|| - d) * ((p1 - p2) / ||p1 - p2||))

    glm::vec3 delta = _p1 - _p2;
    float currentLength = glm::length(delta);

    // Avoid division by zero
    if(currentLength < 0.0001f) return;

    glm::vec3 correctionDir = delta / currentLength;
    float correction = (currentLength - _restLength) / (_w1 + _w2);

    if(_w1 > 0.f) {
        _p1 -= _w1 * correction * correctionDir;
    }

    if(_w2 > 0.f) {
        _p2 += _w2 * correction * correctionDir;
    }
}