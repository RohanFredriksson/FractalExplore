#include <iostream>
#include <GLFW/glfw3.h>

#include "fractal.h"

int main() {

    printf("%f\n", mandelbrot(0.0f, 0.0f, 1));

    GLFWwindow *window; //Pointer tothe GLFW window Object

    if(!glfwInit()) { // Initialize GLFW
        printf("GLFW Not Initialized\n");
        return -1;
    }

    window = glfwCreateWindow(800, 450, "FractalGenerator", 0, 0); // Creating window

    if(!window) { // Check whether window was successfully created
        printf("Window could not be created\n");
        return -1;
    }

    glfwMakeContextCurrent(window);

    while(!glfwWindowShouldClose(window)) {
        glfwPollEvents(); // Updating OpenGL window
        glfwSwapBuffers(window); // Rendering OpenGL window
    }

    glfwTerminate();
    return 0;

}