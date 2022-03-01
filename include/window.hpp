#pragma once

#include <GLFW/glfw3.h>

class Window {

    private:

        // Variables
        static Window* window;
        GLFWwindow* glfwWindow;
        int width;
        int height;
        float fps;

    public:

        Window();

        // Static Methods
        static Window* get();

        // Instance Methods
        void init();
        void loop();
        void exit();

};