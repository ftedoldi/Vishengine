#pragma once

#include "Entity.h"

#include <memory>
#include <vector>

class EntityManager{
public:
    void AddEntity(Entity&& entity);

    void StartEntities() const;

    void UpdateEntities() const;

private:
    std::vector<Entity> _entities;
};