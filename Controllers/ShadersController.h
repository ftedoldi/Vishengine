#pragma once

#include "Shaders/Shader.h"
#include "Components/RenderingComponents.h"

#include <ankerl/unordered_dense.h>

#include <memory>

class ShadersController {
public:
    void AddShader(ShaderID shaderID, std::unique_ptr<Shader> shader);

    Shader* GetShader(ShaderID shaderID);

private:
    ankerl::unordered_dense::map<ShaderID, std::unique_ptr<Shader>> _shaderIdToShader{};
};