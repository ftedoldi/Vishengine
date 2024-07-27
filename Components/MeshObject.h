#pragma once

#include "Mesh.h"

#include <vector>

struct MeshObject {
    std::vector<std::shared_ptr<Mesh>> Meshes{};
};