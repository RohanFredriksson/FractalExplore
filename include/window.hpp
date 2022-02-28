#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>

class Window {

    private:

        static Window* window;
        GLFWwindow* glfwWindow;
        
        int width   = 1920;
        int height  = 1080;
        //char* title = "FractalExplore";
        float fps   = -1.0f;

    public:

        static Window* get() {

            if (Window::window == NULL) {
                Window::window = new Window;
            }

            return Window::window;

        }

        int getWidth() {
            return this->width;
        }

};

Window* Window::window = NULL;