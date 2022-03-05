#pragma once

#include <GLFW/glfw3.h>
#include "camera.hpp"

class Window {

    private:

        static Window* window;
        GLFWwindow* glfwWindow;
        Camera* camera;
        int width;
        int height;
        float fps;

        void init();
        void loop();
        void exit();

    public:

        Window();
        void run();

        static Window* get();
        static int getWidth();
        static int getHeight();
        static void setWidth(int w);
        static void setHeight(int h);
        static float getAspectRatio();
        static float getFPS();
        static Camera* getCamera();

};