#include <iostream>
#include <windows.h>
#include <GLAD/glad.h>
#include <GLFW/glfw3.h>

#include "fractal.h"

int main() {

    printf("%f\n", mandelbrot(0, 0, 255));

    GLFWwindow *window; //Pointer to the GLFW window Object

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