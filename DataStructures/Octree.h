#pragma once

#include "entt/entity/entity.hpp"
#include "glm/vec3.hpp"

#include <list>

namespace Octree {

// Each node of the octree can contain a full mesh. If the mesh is split between two nodes it will be put on the parent node.
struct Node {
    // The center point of the octree node.
    glm::vec3 Center{};

    // The half width of the octree node volume.
    float HalfWidth{};

    // The parent node.
    Node* Parent{};

    // The list of eight children nodes.
    std::array<std::unique_ptr<Node>, 8> Children{};

    // The list of objects contained at this node.
    std::list<entt::entity> Entities{};
};

std::unique_ptr<Node> Build(entt::registry& registry);

void InsertEntity(Node* node, entt::entity entity, entt::registry& registry);

void Update(entt::entity entity, entt::registry& registry);

}
