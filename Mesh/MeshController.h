#pragma once

#include "Components/Mesh.h"

#include "glm/vec2.hpp"
#include "glm/vec3.hpp"

#include <vector>
#include <unordered_map>

struct MeshGpuData {
    uint32_t Vbo{0};
    uint32_t Vao{0};
    uint32_t Ebo{0};
};

struct RawMeshData {
    std::vector<glm::vec3> Vertices{};
    std::vector<uint32_t> Indices{};
    std::vector<glm::vec2> TextureCoords{};
    std::vector<glm::vec3> Normals{};
};

struct MeshData {
    MeshGpuData MeshGpuData{};
    RawMeshData RawMeshData{};
};

class MeshController {
public:
    Mesh CreateMesh(RawMeshData&& rawMeshData);

    const MeshGpuData& GetMeshGpuData(MeshID meshID) const;

    const RawMeshData& GetRawMeshData(MeshID meshID) const;

    const MeshData& GetMeshData(MeshID meshID) const;

    void DeleteMesh(const MeshID& meshID);

private:
    std::unordered_map<MeshID, MeshData> _meshIDToMeshData{};
};