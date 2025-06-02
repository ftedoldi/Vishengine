#pragma once

using MeshID = int;

struct Mesh {
    explicit Mesh(const MeshID meshID) : meshID{meshID} {}

    MeshID meshID{};
};