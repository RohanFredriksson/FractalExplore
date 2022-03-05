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

void WindowResizeListener::resizeCallback(GLFWwindow* window, int screenWidth, int screenHeight) {

    Window::setWidth(screenWidth);
    Window::setHeight(screenHeight);
    Window::getCamera()->adjustProjection();
    //MouseListener::setViewportPos(0.0f, 0.0f);
    //MouseListener::setViewportSize(screenWidth, screenHeight);
    // Re-render the fractal as well
    glViewport(0, 0, screenWidth, screenHeight);

}