#pragma once

#include <glm/glm.hpp>

class DistanceConstraint {
public:
    DistanceConstraint(glm::vec3& p1, glm::vec3& p2, float w1, float w2);

    void Project();

private:
    glm::vec3& _p1;
    glm::vec3& _p2;

    float _w1{};
    float _w2{};

    float _restLength{};
};
