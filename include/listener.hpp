#pragma once

#include <GLFW/glfw3.h>

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