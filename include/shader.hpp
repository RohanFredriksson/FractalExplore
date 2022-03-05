#pragma once

#include <string>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

class Shader {

    private:

        GLuint program;
        bool beingUsed;
        std::string vertexSource;
        std::string fragmentSource;
        const GLchar* vertexCode;
        const GLchar* fragmentCode;

    public:

        Shader(GLchar const* vertexFilepath, GLchar const* fragmentFilepath);
        void compile();
        void use();
        void detach();
        void deleteProgram();

        void uploadMat4(const GLchar* name, const glm::mat4& mat4);

};