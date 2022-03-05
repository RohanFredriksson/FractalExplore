#pragma once

#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

class KeyListener {

    private:

        static KeyListener* instance;
        bool keyPressed[350] = {0};
        bool keyBeginPress[350] = {0};
        KeyListener();

    public:

        static KeyListener* get();
        static void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
        static bool isKeyPressed(int key);
        static bool isBeginPress(int key);

};

class WindowResizeListener {

    public:
        static void resizeCallback(GLFWwindow* window, int screenWidth, int screenHeight);

};