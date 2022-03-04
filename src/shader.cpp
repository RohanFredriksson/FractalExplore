#include <iostream>
#include <string>
#include <fstream>
#include <streambuf>

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include "shader.hpp"

Shader::Shader(GLchar const* vertexFilepath, GLchar const* fragmentFilepath) {

    // Get the vertex and fragment shader source code from their respective files.
    std::string vertexSource;
    std::string fragmentSource;
    std::ifstream vertexFile;
    std::ifstream fragmentFile;

    // Ensures ifstream objects can throw exceptions.
    vertexFile.exceptions (std::ifstream::badbit);
    fragmentFile.exceptions (std::ifstream::badbit);

    try {

        // Open the files.
        vertexFile.open(vertexFilepath);
        fragmentFile.open(fragmentFilepath);

        // Convert the file sources into strings.
        vertexSource = std::string((std::istreambuf_iterator<char>(vertexFile)), std::istreambuf_iterator<char>());
        fragmentSource = std::string((std::istreambuf_iterator<char>(fragmentFile)), std::istreambuf_iterator<char>());

        vertexFile.close();
        fragmentFile.close();

    }

    catch (std::ifstream::failure e) {
        std::cout << "ERROR::SHADER::FILE_NOT_SUCCESSFULLY_READ\n";
    }

    this->vertexCode = vertexSource.c_str();
    this->fragmentCode = fragmentSource.c_str();

};

void Shader::compile() {

    GLuint vertex, fragment;
    GLint success;
    GLchar infoLog[512];

    // Vertex Shader Compilation.
    vertex = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex, 1, &(this->vertexCode), NULL);
    glCompileShader(vertex);

    // Check for compile errors.
    glGetShaderiv(vertex, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(vertex, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
    }

    // Fragment Shader Compilation.
    fragment = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment, 1, &(this->fragmentCode), NULL);
    glCompileShader(fragment);

    // Check for compile errors.
    glGetShaderiv(fragment, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(fragment, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
    }

    // Shader Program Linking.
    this->program = glCreateProgram();
    glAttachShader(this->program, vertex);
    glAttachShader(this->program, fragment);
    glLinkProgram(this->program);

    // Check if any errors occured in linking.
    glGetProgramiv(this->program, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(this->program, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
    }

    glDeleteShader(vertex);
    glDeleteShader(fragment);

};

void Shader::use() {
    if (!this->beingUsed) {
        glUseProgram(this->program);
        this->beingUsed = true;
    }
};

void Shader::detach() {
    glUseProgram(0);
    this->beingUsed = false;
};

void Shader::deleteProgram() {
    glDeleteProgram(this->program);
};