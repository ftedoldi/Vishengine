#include "Shader.h"

#include "ShaderUtils.h"

#include <fstream>
#include <sstream>
#include <stdexcept>

Shader::Shader(const std::string& vertexShaderRelativePath, const std::string& fragmentShaderRelativePath) {
    std::ifstream vShaderFile;
    std::ifstream fShaderFile;
    vShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    fShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    try {
        vShaderFile.open(vertexShaderRelativePath);
        fShaderFile.open(fragmentShaderRelativePath);
        std::stringstream vShaderStream, fShaderStream;
        vShaderStream << vShaderFile.rdbuf();
        fShaderStream << fShaderFile.rdbuf();
        _compileShaders(vShaderStream.str(), fShaderStream.str());
    }
    catch (const std::ifstream::failure& e) {
        throw std::runtime_error{"Failed to read shader files: " + vertexShaderRelativePath + " (" + e.what() + ")"};
    }
}

Shader::~Shader() {
    DeleteProgram();
}

void Shader::DeleteProgram() const {
    glDeleteProgram(_programID);
}

GLint Shader::_uniformLocation(const std::string& name) const {
    const auto it{_locationCache.find(name)};
    if (it != _locationCache.end()) return it->second;
    const GLint loc{glGetUniformLocation(_programID, name.c_str())};
    _locationCache.emplace(name, loc);
    return loc;
}

void Shader::SetUniformVec3(const std::string& uniformName, const float x, const float y, const float z) const {
    glUniform3f(_uniformLocation(uniformName), x, y, z);
}

void Shader::SetUniformVec3(const std::string& uniformName, const glm::vec3 vector) const {
    glUniform3fv(_uniformLocation(uniformName), static_cast<GLsizei>(1), &vector.x);
}

void Shader::SetUniformVec4(const std::string& uniformName, const float x, const float y, const float z, const float w) const {
    glUniform4f(_uniformLocation(uniformName), x, y, z, w);
}

void Shader::SetUniformVec4(const std::string& uniformName, const glm::vec4& vector) const {
    glUniform4fv(_uniformLocation(uniformName), static_cast<GLsizei>(1), &vector.x);
}

void Shader::SetUniformQuat(const std::string& uniformName, const glm::quat& quaternion) const {
    glUniform4fv(_uniformLocation(uniformName), static_cast<GLsizei>(1), &quaternion.x);
}

void Shader::SetUniformMat4(const std::string& uniformName, const glm::mat4& matrix) const {
    glUniformMatrix4fv(_uniformLocation(uniformName), static_cast<GLsizei>(1), false, &matrix[0][0]);
}

void Shader::SetUniformFloat(const std::string& uniformName, const float value) const {
    glUniform1f(_uniformLocation(uniformName), value);
}

void Shader::SetUniformInt(const std::string& uniformName, const int value) const {
    glUniform1i(_uniformLocation(uniformName), value);
}

void Shader::SetBool(const std::string& uniformName, const bool value) const {
    glUniform1i(_uniformLocation(uniformName), value);
}

void Shader::UseProgram() const {
    glUseProgram(_programID);
}

void Shader::_compileShaders(const std::string& vertexShaderCode, const std::string& fragmentShaderCode) {
    const auto* vShaderCode = vertexShaderCode.c_str();
    const auto* fShaderCode = fragmentShaderCode.c_str();

    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vShaderCode, nullptr);
    glCompileShader(vertexShader);
    ShaderUtils::CheckShaderCompileErrors(vertexShader);

    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fShaderCode, nullptr);
    glCompileShader(fragmentShader);
    ShaderUtils::CheckShaderCompileErrors(fragmentShader);

    _programID = glCreateProgram();
    glAttachShader(_programID, vertexShader);
    glAttachShader(_programID, fragmentShader);
    glLinkProgram(_programID);
    ShaderUtils::CheckProgramLinkingErrors(_programID);

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
}