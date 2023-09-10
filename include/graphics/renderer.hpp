#pragma once

#include <GLFW/glfw3.h>

#include "graphics/shader.hpp"
#include "graphics/framebuffer.hpp"

class Renderer {

    private:

        Shader* shader;
        GLuint VAO, VBO, EBO;

    public:

        void start();
        void render();

};