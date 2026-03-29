#pragma once

#include "DataStructures/Octree.h"
#include "Math/Transform.h"
#include "entt/entity/registry.hpp"

class TransformSystem {
public:
    TransformSystem(Octree::Node* octreeRootNode);

    /**
     * \brief Updates the transform of all the entities in the scene if needed.
     * \param registry The registry.
     */
    void Update(entt::registry& registry);

private:
    Transform _getOrComputeWorldTransform(entt::entity entity, entt::registry& registry);

    Octree::Node* _octreeRootNode{};
};

