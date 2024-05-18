#pragma once

#include <glad/gl.h>

#include <iostream>

namespace ShaderUtils{
    void CheckShaderCompileErrors(unsigned int shader);
    void CheckProgramLinkingErrors(unsigned int program);
};