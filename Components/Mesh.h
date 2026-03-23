#pragma once

#include <cstdint>

struct Mesh {
    explicit Mesh(const uint32_t meshID) : meshID{meshID} {}

    uint32_t meshID{};
};