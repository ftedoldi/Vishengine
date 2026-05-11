#pragma once

#include <cstdint>

struct Mesh {
    explicit Mesh(const uint32_t meshID) : MeshID{meshID} {}

    // If the mesh is an instanced mesh the mesh ID is the same as the ID of the actual mesh.
    uint32_t MeshID{};
};