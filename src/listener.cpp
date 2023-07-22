#include <glad/glad.h>
#include "listener.hpp"
#include "camera.hpp"
#include "window.hpp"

namespace {

    bool keyDown[350] = {0};
    bool keyBeginDown[350] = {0};
    int keyPressCount = 0;

    double scrollX = 0.0;
    double scrollY = 0.0;
    double x = 0.0; 
    double y = 0.0; 
    double lastX = 0.0; 
    double lastY = 0.0; 
    Arbitrary worldX(0.0f); 
    Arbitrary worldY (0.0f); 
    Arbitrary lastWorldX(0.0f); 
    Arbitrary lastWorldY(0.0f);
    bool mouseDown[9] = {0};
    bool mouseBeginDown[9] = {0};
    int mouseDownCount = 0;
    bool mouseDragging = 0;
    
}

void KeyListener::keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {

    if (key < 0 || key > 349) {return;}

    if (action == GLFW_PRESS) {
        keyPressCount++;
        keyDown[key] = true;
        keyBeginDown[key] = true;
    }

    else if (action == GLFW_RELEASE) {
        keyPressCount--;
        keyDown[key] = false;
        keyBeginDown[key] = false;
    }

}

bool KeyListener::isKeyDown(int key) {
    if (key < 0 || key > 349) {return false;}
    return keyDown[key];
}

bool KeyListener::isKeyDown() {
    return keyPressCount > 0;
}

bool KeyListener::isKeyBeginDown(int key) {

    if (key < 0 || key > 349) {return false;}

    if (keyBeginDown[key]) {
        keyBeginDown[key] = false;
        return true;
    }

    return false;
}

void MouseListener::calcOrthoX() {
    float currentX = ((float) x / Window::getWidth()) * 2.0f - 1.0f;
    Camera* camera = Window::getCamera();
    worldX = camera->x + Arbitrary(0.5f * currentX) * (camera->width * camera->depth);
}

void MouseListener::calcOrthoY() {
    float currentY = ((float) y / Window::getHeight()) * 2.0f - 1.0f;
    Camera* camera = Window::getCamera();
    worldY = camera->y + Arbitrary(0.5f * currentY) * (camera->height * camera->depth);
}

void MouseListener::mousePosCallback(GLFWwindow* window, double xPos, double yPos) {

    if (mouseDownCount > 0) {
        mouseDragging = true;
    }

    lastX = x;
    lastY = y;
    lastWorldX = worldX;
    lastWorldY = worldY;
    x = xPos;
    y = yPos;
    calcOrthoX();
    calcOrthoY();

}

void MouseListener::mouseButtonCallback(GLFWwindow* window, int button, int action, int mods) {

    if (button > 8 || button < 0) {return;}

    if (action == GLFW_PRESS) {
        mouseDownCount++;
        mouseDown[button] = true;
        mouseBeginDown[button] = true;
    }

    else if (action == GLFW_RELEASE) {
        mouseDownCount--;
        mouseDown[button] = false;
        mouseBeginDown[button] = false;
        mouseDragging = false;
    }

}

void MouseListener::mouseScrollCallback(GLFWwindow* window, double xOffset, double yOffset) {
    scrollX = xOffset;
    scrollY = yOffset;
}

bool MouseListener::isMouseDown(int button) {
    if (button < 0 || button > 9) {return false;}
    return mouseDown[button];
}

bool MouseListener::isMouseDown() {
    return mouseDownCount > 0;
}

bool MouseListener::isMouseBeginDown(int button) {

    if (button < 0 || button > 8) {return false;}

    if (mouseBeginDown[button]) {
        mouseBeginDown[button] = false;
        return true;
    }

    return false;
}

bool MouseListener::hasMouseMoved() {
    return x != lastX || y != lastY;
}

bool MouseListener::isMouseDragging() {
    return mouseDragging;
}

double MouseListener::getX() {
    return x;
}

double MouseListener::getY() {
    return y;
}

double MouseListener::getDx() {
    return x - lastX;
}

double MouseListener::getDy() {
    return y - lastY;
}

Arbitrary MouseListener::getWorldX() {
    return worldX;
}

Arbitrary MouseListener::getWorldY() {
    return worldY;
}

Arbitrary MouseListener::getWorldDx() {
    return worldX - lastWorldX;
}

Arbitrary MouseListener::getWorldDy() {
    return worldY - lastWorldY;
}

double MouseListener::getScrollX() {
    return scrollX;
}

double MouseListener::getScrollY() {
    return scrollY;
}

void MouseListener::endFrame() {

    scrollX = 0.0;
    scrollY = 0.0;
    lastX = x;
    lastY = y;
    lastWorldX = worldX;
    lastWorldY = worldY;
    calcOrthoX();
    calcOrthoY();

}