#pragma once

#include <entt/entt.hpp>

class IRenderPass {
public:
    virtual ~IRenderPass() = default;

    virtual void Execute() = 0;
};
