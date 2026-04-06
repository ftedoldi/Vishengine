#pragma once

#include "Math/Transform.h"
#include "entt/entity/registry.hpp"

class TransformSystem {
public:
    TransformSystem(entt::dispatcher& eventDispatcher);

    /**
     * \brief Updates the transform of all the entities in the scene if needed.
     * \param registry The registry.
     */
    void Update(entt::registry& registry) const;

private:
    void _updateTransform(entt::entity entity, entt::registry& registry) const;

    entt::dispatcher& _eventDispatcher;
};

