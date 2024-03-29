#include <iostream>
#include <cstdlib>
#include <glad/glad.h>
#include <glm/gtc/type_ptr.hpp>

#include "graphics/shader.hpp"

Shader::Shader(std::string vertex, std::string fragment) {

    const char* vertexCode = vertex.c_str();
    const char* fragmentCode = fragment.c_str();

    GLuint v, f;
    GLint success;
    GLchar infoLog[512];

    // Vertex shader compilation.
    v = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(v, 1, (char const * const *)&(vertexCode), nullptr);
    glCompileShader(v);

    // Check for compilation errors.
    glGetShaderiv(v, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(v, 512, nullptr, infoLog);
        std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << "\n";
    }

    // Fragment shader compilation.
    f = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(f, 1, (char const * const *)&(fragmentCode), nullptr);
    glCompileShader(f);

    // Check for compilation errors.
    glGetShaderiv(f, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(f, 512, nullptr, infoLog);
        std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << "\n";
    }

    // Shader program linking.
    this->program = glCreateProgram();
    glAttachShader(this->program, v);
    glAttachShader(this->program, f);
    glLinkProgram(this->program);

    // Check if any errors occurred in linking.
    glGetProgramiv(this->program, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(this->program, 512, nullptr, infoLog);
        std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << "\n";
    }

    glDeleteShader(v);
    glDeleteShader(f);

}

Shader::~Shader() {
    glDeleteProgram(this->program);
}

void Shader::bind() {
    glUseProgram(this->program);
}

void Shader::unbind() {
    glUseProgram(0);
}

void Shader::uploadInt(std::string name, int value) {
    this->bind();
    GLint location = glGetUniformLocation(this->program, name.c_str());
    glUniform1i(location, value);
}

void Shader::uploadFloat(std::string name, float value) {
    this->bind();
    GLint location = glGetUniformLocation(this->program, name.c_str());    
    glUniform1f(location, value);
}

void Shader::uploadVec2(std::string name, glm::vec2 vector) {
    this->bind();
    GLint location = glGetUniformLocation(this->program, name.c_str());
    glUniform2f(location, vector.x, vector.y);
}

void Shader::uploadVec3(std::string name, glm::vec3 vector) {
    this->bind();
    GLint location = glGetUniformLocation(this->program, name.c_str());
    glUniform3f(location, vector.x, vector.y, vector.z);
}

void Shader::uploadVec4(std::string name, glm::vec4 vector) {
    this->bind();
    GLint location = glGetUniformLocation(this->program, name.c_str());
    glUniform4f(location, vector.x, vector.y, vector.z, vector.w);
}

void Shader::uploadMat4(std::string name, glm::mat4 matrix) {
    this->bind();
    GLint location = glGetUniformLocation(this->program, name.c_str());
    glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(matrix));
}

void Shader::uploadTexture(std::string name, int slot) {
    this->bind();
    GLint location = glGetUniformLocation(this->program, name.c_str());
    glUniform1i(location, slot);
}

void Shader::uploadIntArray(std::string name, int num, int* array) {
    this->bind();
    GLint location = glGetUniformLocation(this->program, name.c_str());
    glUniform1iv(location, num, array);
}

void Shader::uploadUnsignedIntArray(std::string name, int num, unsigned int* array) {
    this->bind();
    GLint location = glGetUniformLocation(this->program, name.c_str());
    glUniform1uiv(location, num, array);
}