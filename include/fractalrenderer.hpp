#pragma once

#include <GLFW/glfw3.h>
#include "shader.hpp"

class FractalRenderer {

    private:
        
        Shader* shader;
        GLuint VAO, VBO, EBO;

    public:

        FractalRenderer(Shader* shader);
        void start();
        void render();

};