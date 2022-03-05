#include <GLFW/glfw3.h>
#include "listener.hpp"

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