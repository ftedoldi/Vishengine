#pragma once

#include <ankerl/unordered_dense.h>

#include <glad/gl.h>
#include <glm/glm.hpp>
#include "glm/detail/type_quat.hpp"
#include <string>

class Shader {
public:
    Shader(const std::string& vertexShaderRelativePath, const std::string& fragmentShaderRelativePath);

    ~Shader();

    Shader(const Shader&) = delete;
    Shader& operator=(const Shader&) = delete;
    Shader(Shader&&) = delete;
    Shader& operator=(Shader&&) = delete;

	void UseProgram() const;

	void DeleteProgram() const;

	void SetUniformVec3(const std::string& uniformName, float x, float y, float z) const;

	void SetUniformVec3(const std::string& uniformName, glm::vec3 vector) const;

	void SetUniformVec4(const std::string& uniformName, float x, float y, float z, float w) const;

	void SetUniformVec4(const std::string& uniformName, const glm::vec4& vector) const;

    void SetUniformQuat(const std::string& uniformName, const glm::quat& quaternion) const;

	void SetUniformMat4(const std::string& uniformName, const glm::mat4& matrix) const;

	void SetUniformFloat(const std::string& uniformName, float value) const;

	void SetUniformInt(const std::string& uniformName, int value) const;

    void SetBool(const std::string& uniformName, bool value) const;

private:
    [[nodiscard]] GLint _uniformLocation(const std::string& name) const;

	unsigned int _programID{0};

    mutable ankerl::unordered_dense::map<std::string, GLint> _locationCache{};

	void _compileShaders(const std::string& vertexShaderCode, const std::string& fragmentShaderCode);
};

