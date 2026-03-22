#pragma once

#include <cstdint>

using MeshID = uint32_t;

struct InstancedMesh {
    explicit InstancedMesh(const MeshID meshID) : meshID{meshID} {}

    MeshID meshID{};
};