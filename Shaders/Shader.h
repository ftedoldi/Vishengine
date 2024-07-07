#pragma once

#include <glad/gl.h>

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

#include <string>

class Shader {
public:
    Shader(const std::string&vertexShaderRelativePath, const std::string&fragmentShaderRelativePath);

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

private:
	unsigned int _programID = 0;

	void _compileShaders(const std::string& vertexShaderCode, const std::string& fragmentShaderCode);
};

