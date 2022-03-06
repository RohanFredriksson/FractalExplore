#include <iostream>
#include <stdlib.h>

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

#include "camera.hpp"
#include "shader.hpp"
#include "window.hpp"
#include "listener.hpp"
#include "fractalrenderer.hpp"

Window::Window() {

    this->width = 800;
    this->height = 800;
    this->fps = -1.0f;

	this->camera = new Camera(glm::vec2(0.0f, 0.0f));

}

Window* Window::get() {

    if (Window::window == NULL) {
        Window::window = new Window;
    }

    return Window::window;

}

void Window::init() {

    // Initialise GLFW
	if (!glfwInit()) {
		std::cout << "Unable to initialise GLFW." << std::endl;
		return;
	}

	// Configure GLFW
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	// Create the window
	this->glfwWindow = glfwCreateWindow(800, 800, "Fractal Explore", NULL, NULL);
	if (this->glfwWindow == NULL) {
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return;
	}

	// Manage callbacks
	glfwSetKeyCallback(this->glfwWindow, KeyListener::keyCallback);
	glfwSetWindowSizeCallback(this->glfwWindow, WindowResizeListener::resizeCallback);

	// Make the OpenGl context current
	glfwMakeContextCurrent(this->glfwWindow);

	//Enable v-sync
    glfwSwapInterval(1);

	//Load GLAD so it configures OpenGL
	gladLoadGL();

	// Call the resize callback for initialisation.
	WindowResizeListener::resizeCallback(this->glfwWindow, 800, 800);

}

void Window::loop() {

    const char* v = "assets/shaders/mandelbrot.vert";
	const char* f = "assets/shaders/mandelbrot.frag";
	Shader mandelbrot(v, f);
	mandelbrot.compile();

	FractalRenderer renderer(&mandelbrot);
	renderer.start();

	// Main while loop
	while (!glfwWindowShouldClose(this->glfwWindow)) {	

		glfwPollEvents();

		glClearColor(0.015625f, 0.015625f, 0.015625f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		renderer.render();

		glfwSwapBuffers(this->glfwWindow);

	}

    mandelbrot.deleteProgram();
    
}

void Window::exit() {

	// Delete window before ending the program
	glfwDestroyWindow(this->glfwWindow);
	// Terminate GLFW before ending the program
	glfwTerminate();

}

void Window::run() {

	this->init();
	this->loop();
	this->exit();
	
}

int Window::getWidth() {
	return Window::get()->width;
}

int Window::getHeight() {
	return Window::get()->height;
}

void Window::setWidth(int w) {
	Window::get()->width = w;
}

void Window::setHeight(int h) {
	Window::get()->height = h;
}

float Window::getAspectRatio() {
	return (float) Window::getWidth() / (float) Window::getHeight();
}

float Window::getFPS() {
	return Window::get()->fps;
}

Camera* Window::getCamera() {
	return Window::get()->camera;
}

Window* Window::window = NULL;