#include "EntityManager.h"

void EntityManager::AddEntity(Entity&& entity) {
    _entities.push_back(std::move(entity));
}

void EntityManager::StartEntities() const {
    for(const auto& entity: _entities){
        entity.StartComponents();
    }
}

void EntityManager::UpdateEntities() const {
    for(const auto& entity: _entities){
        entity.UpdateComponents();
    }
}
