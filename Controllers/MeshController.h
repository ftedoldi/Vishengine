#pragma once

#include "Components/Mesh.h"
#include "Events/GameEvents.h"

#include "glm/vec2.hpp"
#include "glm/vec3.hpp"
#include "glm/vec4.hpp"

#include <unordered_map>
#include <vector>

struct MeshGpuData {
    uint32_t Vbo{0};
    uint32_t Vao{0};
    uint32_t Ebo{0};
    uint32_t InstanceSsbo{0};
    uint32_t IndexCount{0};
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

struct InstanceData {
    glm::vec4 PositionAndScale{};
    glm::vec4 Rotation{};
};

class MeshController {
public:
    explicit MeshController(entt::registry& registry);

    Mesh CreateMesh(RawMeshData&& rawMeshData);

    [[nodiscard]] const MeshGpuData& GetMeshGpuData(uint32_t meshID) const;

    [[nodiscard]] const RawMeshData& GetRawMeshData(uint32_t meshID) const;

    [[nodiscard]] const MeshData& GetMeshData(uint32_t meshID) const;

    void DeleteMesh(uint32_t meshID);

private:
    void _onMeshBeginDeletion(entt::entity meshNodeEntity);

    std::unordered_map<uint32_t, MeshData> _meshIDToMeshData{};

    uint32_t _nextMeshId{0};

    entt::registry& _registry;
};