#pragma once

#include <GLFW/glfw3.h>
#include "camera.hpp"

namespace Window {

    int getWidth();
    int getHeight();
    float getAspectRatio();
    Camera* getCamera();

};

namespace KeyListener {

    void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
    bool isKeyDown(int key);
    bool isKeyDown();
    bool isKeyBeginDown(int key);

}

namespace MouseListener {

    void calcOrthoX();
    void calcOrthoY();
    void mousePosCallback(GLFWwindow* window, double xPos, double yPos);
    void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods);
    void mouseScrollCallback(GLFWwindow* window, double xOffset, double yOffset);
    bool isMouseDown(int button);
    bool isMouseDown();
    bool isMouseBeginDown(int button);
    bool hasMouseMoved();
    bool isMouseDragging();
    double getX();
    double getY();
    double getDx();
    double getDy();
    Arbitrary getWorldX();
    Arbitrary getWorldY();
    Arbitrary getWorldDx();
    Arbitrary getWorldDy();
    double getScrollX();
    double getScrollY();
    void endFrame();

}