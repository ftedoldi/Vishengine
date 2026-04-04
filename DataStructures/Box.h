#pragma once

#include "glm/vec3.hpp"

struct Box {
    glm::vec3 Min{};

    glm::vec3 Max{};

    Box() = default;

    Box(const glm::vec3 min, const glm::vec3 max) : Min{min}, Max{max} {}

    Box(const std::vector<glm::vec3>& vertices) {
        Min = { std::numeric_limits<float>::max(),
            std::numeric_limits<float>::max(),
            std::numeric_limits<float>::max() };
        Max = { std::numeric_limits<float>::lowest(),
                    std::numeric_limits<float>::lowest(),
                    std::numeric_limits<float>::lowest() };

        for (const auto v : vertices) {
            Min.x = std::min(Min.x, v.x);
            Min.y = std::min(Min.y, v.y);
            Min.z = std::min(Min.z, v.z);

            Max.x = std::max(Max.x, v.x);
            Max.y = std::max(Max.y, v.y);
            Max.z = std::max(Max.z, v.z);
        }
    }

    Box(const glm::vec3 center, const float halfWidth) : Min{center - halfWidth}, Max{center + halfWidth} {}

    [[nodiscard]] glm::vec3 GetCenter() const {
        return (Min + Max) * 0.5f;
    }

    [[nodiscard]] glm::vec3 GetExtent() const {
        return (Max - Min) * 0.5f;
    }

    [[nodiscard]] glm::vec3 GetSize() const {
        return Max - Min;
    }

    [[nodiscard]] std::array<glm::vec3, 8> GetVertices() const {
        return {
            glm::vec3{Min.x, Min.y, Min.z},
            glm::vec3{Max.x, Min.y, Min.z},
            glm::vec3{Min.x, Max.y, Min.z},
            glm::vec3{Max.x, Max.y, Min.z},
            glm::vec3{Min.x, Min.y, Max.z},
            glm::vec3{Max.x, Min.y, Max.z},
            glm::vec3{Min.x, Max.y, Max.z},
            glm::vec3{Max.x, Max.y, Max.z}
        };
    }

    [[nodiscard]] bool Intersect(const Box& other) const {
        if ((Min.x > other.Max.x) || (other.Min.x > Max.x)){
            return false;
        }

        if ((Min.y > other.Max.y) || (other.Min.y > Max.y)){
            return false;
        }

        if ((Min.z > other.Max.z) || (other.Min.z > Max.z)){
            return false;
        }

        return true;
    }

};