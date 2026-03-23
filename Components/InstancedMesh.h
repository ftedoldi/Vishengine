#pragma once

#include <cstdint>

struct InstancedMesh {
    explicit InstancedMesh(const uint32_t meshID) : meshID{meshID} {}

    uint32_t meshID{};
};