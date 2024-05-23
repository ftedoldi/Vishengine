#pragma once

#include "Component.h"

#include <vector>
#include <memory>
#include <random>
#include <type_traits>

class Entity
{
public:
	Entity();

	void AddComponent(std::unique_ptr<Component> component);

    void StartComponents() const;
    void UpdateComponents() const;

    template<typename T, std::enable_if_t<std::is_base_of_v<Component, T>, bool> = true>
    void AddComponentByClass(){
        _components.emplace_back(std::make_unique<T>());
    }

    template<typename T, std::enable_if_t<std::is_base_of_v<Component, T>, bool> = true>
    T* FindComponentByClass() const{
        for(const auto& currentComponent: _components){
            if(dynamic_cast<T*>(currentComponent.get())){
                return currentComponent.get();
            }
        }
        return nullptr;
    }

private:
    std::vector<std::unique_ptr<Component>> _components{};

    int _id{0};
};