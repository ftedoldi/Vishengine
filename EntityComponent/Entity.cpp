#include "Entity.h"
#include "Mesh.h"

Entity::Entity(){
    std::random_device rd;
    std::mt19937 rng(rd());
    std::uniform_int_distribution<int> uni;

    _id = uni(rng);
}

void Entity::StartComponents() const {
    for(const auto& component: _components){
        component->Start();
    }
}

void Entity::UpdateComponents() const {
    for(const auto& component: _components){
        component->Update();
    }
}

void Entity::AddComponent(std::unique_ptr<Component> component){
	_components.push_back(std::move(component));
}
