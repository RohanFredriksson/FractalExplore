#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

class Shader {

    private:

        GLuint program;
        bool beingUsed;
        const GLchar* vertexCode;
        const GLchar* fragmentCode;

    public:

        Shader(GLchar const* vertexFilepath, GLchar const* fragmentFilepath);
        void compile();
        void use();
        void detach();
        void deleteProgram();

};