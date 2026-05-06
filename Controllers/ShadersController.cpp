#include "ShadersController.h"

void ShadersController::AddShader(const uint32_t shaderID, std::unique_ptr<Shader> shader) {
    _shaderIdToShader.emplace(shaderID, std::move(shader));
}

Shader* ShadersController::GetShader(const uint32_t shaderID) {
    assert(_shaderIdToShader.contains(shaderID));
    return _shaderIdToShader[shaderID].get();
}