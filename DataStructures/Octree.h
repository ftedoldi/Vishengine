#pragma once

#include "Box.h"
#include "entt/entity/entity.hpp"
#include "glm/vec3.hpp"

#include <array>
#include <list>
#include <memory>

namespace OC {
// Each node of the octree can contain a full mesh. If the mesh is split between two nodes it will be put on the parent node.
struct Node {
    // The center point of the octree node.
    glm::vec3 Center{};

    // The half width of the octree node volume.
    glm::vec3 HalfWidth{};

    // The parent node.
    Node* Parent{};

    // The list of eight children nodes.
    std::array<std::unique_ptr<Node>, 8> Children{};

    // The list of objects contained at this node.
    std::list<entt::entity> Entities{};
};

}

class Octree {
public:
    void Init(entt::registry& registry, int32_t maxDepth);

    // Public entry point: inserts entity starting from the given node at full allowed depth.
    void InsertEntity(OC::Node* node, entt::entity entity, entt::registry& registry) const;

    void Update(entt::entity entity, entt::registry& registry);

    [[nodiscard]] OC::Node* GetRootNode() const;

private:
    // Expand the octree root until it contains the given world space bounding box.
    void _expand(const Box& entityBox);

    // Walk up from 'node' and prune any children whose entire subtree is empty.
    void _shrink(OC::Node* node) const;

    int32_t _maxDepth{};

    std::unique_ptr<OC::Node> _rootNode{};
};
