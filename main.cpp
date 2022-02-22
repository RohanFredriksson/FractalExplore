#include <iostream>
#include <GLFW/glfw3.h>

int main() {

    GLFWwindow *window; //Pointer tothe GLFW window Object

    if(!glfwInit()) { // Initialize GLFW
        std::cout << "GLFW Not Initialized" << std::endl;
        return -1;
    }

    window = glfwCreateWindow(800, 450, "FractalGenerator", 0, 0); // Creating window

    if(!window) { // Check whether window was successfully created
        std::cout << "Window wasnt created" << std::endl;
        return -1;
    }

    glfwMakeContextCurrent(window);

    while(true) {
        glfwPollEvents(); // Updating OpenGL window
        glfwSwapBuffers(window); // Rendering OpenGL window
    }

    glfwTerminate();
    return 0;

}