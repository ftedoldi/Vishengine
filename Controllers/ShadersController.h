#pragma once

#include "../Shaders/Shader.h"

#include <memory>
#include <unordered_map>

class ShadersController {
public:
    void AddShader(uint32_t shaderID, std::unique_ptr<Shader> shader);

    Shader* GetShader(uint32_t shaderID);

private:
    std::unordered_map<uint32_t, std::unique_ptr<Shader>> _shaderIdToShader{};
};