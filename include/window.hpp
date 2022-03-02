#pragma once

#include <GLFW/glfw3.h>

class Window {

    private:

        static Window* window;
        GLFWwindow* glfwWindow;
        int width;
        int height;
        float fps;

        void init();
        void loop();
        void exit();

    public:

        Window();
        static Window* get();
        void run();    

};