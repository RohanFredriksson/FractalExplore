#pragma once

#include <string>
#include <fstream>
#include <iostream>

class Shader {

    private:

        GLuint program;
        GLChar* vertexCode;
        GLChar* fragmentCode;
        bool beingUsed;

    public:

        Shader(const GLchar* vertexFilepath, const GLChar* fragmentFilepath) {

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
                printf("ERROR::SHADER::FILE_NOT_SUCCESSFULLY_READ\n");
            }

            this.vertexCode = vertexCode.c_str();
            this.fragmentCode = fragmentCode.c_str();

        }

        void compile() {

        }

        void use() {

        }

        void detach() {
            
        }
}