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

class MouseListener {

    private:

        static MouseListener* instance;
        double scrollX, scrollY;
        double xPos, yPos, lastX, lastY, worldX, worldY, lastWorldX, lastWorldY;
        bool mouseButtonPressed[9] = {0};
        bool isDraggings = 0;
        int mouseButtonsDown = 0;
        glm::vec2 viewportPos = glm::vec2(0.0f, 0.0f);
        glm::vec2 viewportSize = glm::vec2(0.0f, 0.0f);
        
        MouseListener();
        static void calcOrthoX();
        static void calcOrthoY();

    public:

        static MouseListener* get();
        static void mousePosCallback(GLFWwindow* window, double xPos, double yPos);
        static void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods);
        static void mouseScrollCallback(GLFWwindow* window, double xOffset, double yOffset);
        static void endFrame();
        static float getX();
        static float getY();
        static float getWorldDx();
        static float getWorldDy();
        static float getDx();
        static float getDy();
        static float getScrollX();
        static float getScrollY();
        static bool isDragging();
        static bool mouseButtonDown(int button);
        static float getOrthoX();
        static float getOrthoY();
        static void setViewportPos(float x, float y);
        static void setViewportSize(float x, float y);
        
};