#pragma once

#include <cstdint>

using MeshID = uint32_t;

struct Mesh {
    explicit Mesh(const MeshID meshID) : meshID{meshID} {}

    MeshID meshID{};
};