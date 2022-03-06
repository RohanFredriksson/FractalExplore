#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "listener.hpp"
#include "window.hpp"

KeyListener::KeyListener() {

}

KeyListener* KeyListener::get() {

    if  (KeyListener::instance == NULL) {
         KeyListener::instance = new KeyListener;
    }

    return KeyListener::instance;

}

void KeyListener::keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {

    if (action == GLFW_PRESS) {
        KeyListener::get()->keyPressed[key] = true;
        KeyListener::get()->keyBeginPress[key] = true;
    }

    else if (action == GLFW_RELEASE) {
        KeyListener::get()->keyPressed[key] = false;
        KeyListener::get()->keyBeginPress[key] = false;
    }

}

bool KeyListener::isKeyPressed(int key) {
    return KeyListener::get()->keyPressed[key];
}

bool KeyListener::isBeginPress(int key) {

    bool result = KeyListener::get()->keyBeginPress[key];
    if (result) {
        KeyListener::get()->keyBeginPress[key] = false;
    }

    return result;
}

KeyListener* KeyListener::instance = NULL;

MouseListener::MouseListener() {

    this->scrollX = 0.0;
    this->scrollY = 0.0;
    this->xPos    = 0.0;
    this->yPos    = 0.0;
    this->lastX   = 0.0;
    this->lastY   = 0.0;

}

MouseListener* MouseListener::get() {
    
    if  (MouseListener::instance == NULL) {
         MouseListener::instance = new MouseListener;
    }

    return MouseListener::instance;

}

void MouseListener::mousePosCallback(GLFWwindow* window, double xPos, double yPos) {
    
    if (MouseListener::get()->mouseButtonsDown > 0) {
        MouseListener::get()->isDraggings = true;
    }

    MouseListener::get()->lastX = MouseListener::get()->xPos;
    MouseListener::get()->lastY = MouseListener::get()->yPos;
    MouseListener::get()->lastWorldX = MouseListener::get()->worldX;
    MouseListener::get()->lastWorldY = MouseListener::get()->worldY;
    MouseListener::get()->xPos = xPos;
    MouseListener::get()->yPos = yPos;
    get()->calcOrthoX();
    get()->calcOrthoY();

}

void MouseListener::mouseButtonCallback(GLFWwindow* window, int button, int action, int mods) {
    
    if (action == GLFW_PRESS) {
        
        MouseListener::get()->mouseButtonsDown++;

        if (button <  9) {
            MouseListener::get()->mouseButtonPressed[button] = true;
        }

    }

    else if (action == GLFW_RELEASE) {

        MouseListener::get()->mouseButtonsDown--;

        if (button < 9) {
            MouseListener::get()->mouseButtonPressed[button] = false;
            MouseListener::get()->isDraggings = false;
        }

    }

}

void MouseListener::mouseScrollCallback(GLFWwindow* window, double xOffset, double yOffset) {

    MouseListener::get()->scrollX = xOffset;
    MouseListener::get()->scrollY = yOffset;

}

void MouseListener::endFrame() {
    
    MouseListener::get()->scrollX = 0;
    MouseListener::get()->scrollY = 0;
    MouseListener::get()->lastX = MouseListener::get()->xPos;
    MouseListener::get()->lastY = MouseListener::get()->yPos;
    MouseListener::get()->lastWorldX = MouseListener::get()->worldX;
    MouseListener::get()->lastWorldY = MouseListener::get()->worldY;
    get()->calcOrthoX();
    get()->calcOrthoY();

}

float MouseListener::getX() {
    return (float)MouseListener::get()->xPos;
}

float MouseListener::getY() {
    return (float)MouseListener::get()->yPos;
}

float MouseListener::getWorldDx() {
    return (float)(MouseListener::get()->lastWorldX - MouseListener::get()->worldX);
}

float MouseListener::getWorldDy() {
    return (float)(MouseListener::get()->lastWorldY - MouseListener::get()->worldY);
}

float MouseListener::getDx() {
    return (float)(MouseListener::get()->lastX - MouseListener::get()->xPos);
}

float MouseListener::getDy() {
    return (float)(MouseListener::get()->lastY - MouseListener::get()->yPos);
}

float MouseListener::getScrollX() {
    return (float)MouseListener::get()->scrollX;
}

float MouseListener::getScrollY() {
    return (float)MouseListener::get()->scrollY;
}

bool MouseListener::isDragging() {
    return MouseListener::get()->isDraggings;
}

bool MouseListener::mouseButtonDown(int button) {

    if (button < 9) {
        return MouseListener::get()->mouseButtonPressed[button];
    }
    return false;

}

float MouseListener::getOrthoX() {
    return (float)MouseListener::get()->worldX;
}

float MouseListener::getOrthoY() {
    return (float)MouseListener::get()->worldY;
}

void MouseListener::calcOrthoX() {
    
    float currentX = MouseListener::getX() - MouseListener::get()->viewportPos.x;
    currentX = (currentX / MouseListener::get()->viewportSize.x) * 2.0f - 1.0f;
    glm::vec4 tmp(currentX, 0, 0, 1);
    glm::mat4 viewProjection = Window::getCamera()->getInverseView() * Window::getCamera()->getInverseProjection();
    tmp = viewProjection * tmp;

    MouseListener::get()->worldX = tmp.x;

}

void MouseListener::calcOrthoY() {
    
    float currentY = MouseListener::getY() - MouseListener::get()->viewportPos.y;
    currentY = (currentY / MouseListener::get()->viewportSize.y) * 2.0f - 1.0f;
    glm::vec4 tmp(0, currentY, 0, 1);
    glm::mat4 viewProjection = Window::getCamera()->getInverseView() * Window::getCamera()->getInverseProjection();
    tmp = viewProjection * tmp;

    MouseListener::get()->worldY = tmp.y;

}

void MouseListener::setViewportPos(float x, float y) {

    MouseListener::get()->viewportPos.x = x;
    MouseListener::get()->viewportPos.y = y;

}

void MouseListener::setViewportSize(float x, float y) {
    
    MouseListener::get()->viewportSize.x = x;
    MouseListener::get()->viewportSize.y = y;
    
}

MouseListener* MouseListener::instance = NULL;

void WindowResizeListener::resizeCallback(GLFWwindow* window, int screenWidth, int screenHeight) {

    Window::setWidth(screenWidth);
    Window::setHeight(screenHeight);
    Window::getCamera()->adjustProjection();
    MouseListener::setViewportPos(0.0f, 0.0f);
    MouseListener::setViewportSize(screenWidth, screenHeight);
    glViewport(0, 0, screenWidth, screenHeight);

}