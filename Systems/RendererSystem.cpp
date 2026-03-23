#include "RendererSystem.h"

void RendererSystem::AddPass(std::unique_ptr<IRenderPass> pass) {
    _passes.push_back(std::move(pass));
}

void RendererSystem::Update() const {
    for (const auto& pass : _passes) {
        pass->Execute();
    }
}
