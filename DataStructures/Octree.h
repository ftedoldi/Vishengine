#pragma once

#include "entt/entity/entity.hpp"
#include "glm/vec3.hpp"

namespace Octree {

    // Each node of the octree can contain a full mesh. If the mesh is split between two nodes it will be put on the parent node.
    struct Node {
        // The center point of the octree node.
        glm::vec3 Center{};

        // The half width of the octree node volume.
        float HalfWidth{};

        // The list of eight children nodes.
        std::array<std::unique_ptr<Node>, 8> Children{};

        // The list of objects contained at this node.
        std::vector<entt::entity> EntityList{};
    };

    std::unique_ptr<Node> Build(entt::registry& registry);

    /**
     * \brief Preallocates an octree down to a specific depth.
     * \param center The center of the root node.
     * \param halfWidth The half width of the root node.
     * \param stopDepth The stop depth.
     * \return A pointer to the root node.
     */
    std::unique_ptr<Node> BuildOctree(glm::vec3 center, float halfWidth, int32_t stopDepth);

    void InsertEntity(Node* node, entt::entity entity, entt::registry& registry);

}
