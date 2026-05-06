#include "ShadersController.h"

#include <ranges>

void ShadersController::AddShader(const ShaderID shaderID, std::unique_ptr<Shader> shader) {
    _shaderIdToShader.emplace(shaderID, std::move(shader));
}

Shader* ShadersController::GetShader(const ShaderID shaderID) {
    assert(_shaderIdToShader.contains(shaderID));
    return _shaderIdToShader[shaderID].get();
}