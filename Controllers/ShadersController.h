#pragma once

#include "Shaders/Shader.h"
#include "Components/RenderingComponents.h"

#include <memory>
#include <unordered_map>

class ShadersController {
public:
    void AddShader(ShaderID shaderID, std::unique_ptr<Shader> shader);

    Shader* GetShader(ShaderID shaderID);

private:
    std::unordered_map<ShaderID, std::unique_ptr<Shader>> _shaderIdToShader{};
};