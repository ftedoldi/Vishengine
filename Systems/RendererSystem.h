#pragma once

#include "IRenderPass.h"

#include <memory>
#include <vector>

class RendererSystem {
public:
    RendererSystem() = default;

    void AddPass(std::unique_ptr<IRenderPass> pass);

    void Update() const;

private:
    std::vector<std::unique_ptr<IRenderPass>> _passes{};
};
