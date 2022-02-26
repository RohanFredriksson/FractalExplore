#pragma once

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

class Shader {

    private:

        GLuint program;
        bool beingUsed;
        const GLchar* vertexCode;
        const GLchar* fragmentCode;

    public:

        Shader(GLchar const* vertexFilepath, GLchar const* fragmentFilepath) {

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
                std::stringstream vertexStream, fragmentStream;

                // Read file's buffer into streams.
                vertexStream << vertexFile.rdbuf();
                fragmentStream << fragmentFile.rdbuf();
                vertexFile.close();
                fragmentFile.close();

                // Convert the streams into strings.
                vertexSource = vertexStream.str();
                fragmentSource = fragmentStream.str();

            }

            catch (std::ifstream::failure e) {
                std::cout << "ERROR::SHADER::FILE_NOT_SUCCESSFULLY_READ\n";
            }

            this->vertexCode = vertexSource.c_str();
            this->fragmentCode = fragmentSource.c_str();

        }

        void compile() {

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

        }

        void use() {
            if (!this->beingUsed) {
                glUseProgram(this->program);
                this->beingUsed = true;
            }
        }

        void detach() {
            glUseProgram(0);
            this->beingUsed = false;
        }

        void deleteProgram() {
            glDeleteProgram(this->program);
        }

};