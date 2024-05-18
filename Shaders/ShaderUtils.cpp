#include "ShaderUtils.h"

namespace ShaderUtils{
    void CheckShaderCompileErrors(const unsigned int shader){
        int success;
        char infoLog[1024];
        glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
        if (!success)
        {
            glGetShaderInfoLog(shader, 1024, nullptr, infoLog);
            std::cout << "ERROR::SHADER_COMPILATION_ERROR: \n" << infoLog << "\n" << std::endl;
        }
    }

    void CheckProgramLinkingErrors(const unsigned int program){
        int success;
        char infoLog[1024];
        glGetProgramiv(program, GL_LINK_STATUS, &success);
        if (!success)
        {
            glGetProgramInfoLog(program, 1024, nullptr, infoLog);
            std::cout << "ERROR::PROGRAM_LINKING_ERROR of type: \n" << infoLog << "\n" << std::endl;
        }
    }
}
