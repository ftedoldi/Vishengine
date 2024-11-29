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

    // Avoid divisions by zero
    if(currentLength < 0.0001f) return;
    if(_w1 + _w2 < 0.0001f) return;

    float deltaLength{currentLength - _restLength};
    glm::vec3 correctionDir = delta / currentLength;

    float deltaP1Weights{-_w1 / (_w1 + _w2)};
    _p1 += deltaP1Weights * deltaLength * correctionDir;

    float deltaP2Weights{_w2 / (_w1 + _w2)};
    _p2 += deltaP2Weights * deltaLength * correctionDir;
}