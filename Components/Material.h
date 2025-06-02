#pragma once

using MaterialID = int;

struct Material {
    explicit Material(const MaterialID materialId) : materialID{materialId} {}

    MaterialID materialID{};
};