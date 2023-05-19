#include <iostream>
#include <stdlib.h>

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

#include "camera.hpp"
#include "shader.hpp"
#include "window.hpp"
#include "listener.hpp"
#include "framebuffer.hpp"
#include "renderer.hpp"

Window::Window() {

    this->width = 800;
    this->height = 800;
    this->fps = -1.0f;

	this->camera = new Camera(glm::vec2(-0.75f, 0.0f));

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
	glfwSetCursorPosCallback(this->glfwWindow, MouseListener::mousePosCallback);
	glfwSetMouseButtonCallback(this->glfwWindow, MouseListener::mouseButtonCallback);
	glfwSetScrollCallback(this->glfwWindow, MouseListener::mouseScrollCallback);

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

	// Shaders
	Shader mandelbrot("assets/shaders/mandelbrot.vert", "assets/shaders/mandelbrot.frag");
	Shader hsv("assets/shaders/hsv.vert", "assets/shaders/hsv.frag");

	// Framebuffer
	Framebuffer framebuffer(GL_RGB, this->width, this->height, GL_RGB, GL_UNSIGNED_BYTE);

	// Renderer
	Renderer renderer;
	renderer.start();
	
	// Main while loop
	while (!glfwWindowShouldClose(this->glfwWindow)) {	

		glfwPollEvents();
		
		if (MouseListener::isDragging()) {
			if (MouseListener::getDx() != 0) {this->camera->position.x = this->camera->position.x + (MouseListener::getWorldDx());}
			if (MouseListener::getDy() != 0) {this->camera->position.y = this->camera->position.y - (MouseListener::getWorldDy());}
		}

		if (MouseListener::getScrollY() != 0.0f) {

			if (MouseListener::getScrollY() > 0) {
				this->camera->setZoom(this->camera->getZoom() * 1.1f);
				this->camera->position.x = this->camera->position.x + (MouseListener::getOrthoX() - this->camera->position.x) * 0.0909090909f;
				this->camera->position.y = this->camera->position.y - (MouseListener::getOrthoY() - this->camera->position.y) * 0.0909090909f;
			}

			else {
				this->camera->setZoom(this->camera->getZoom() / 1.1f);
				this->camera->position.x = this->camera->position.x - (MouseListener::getOrthoX() - this->camera->position.x) * 0.1f;
				this->camera->position.y = this->camera->position.y + (MouseListener::getOrthoY() - this->camera->position.y) * 0.1f;
			}

			this->camera->adjustProjection();
			
		}

		// Render grayscale mandelbrot set to framebuffer.
		framebuffer.bind();
		mandelbrot.uploadVec2("uPosition", this->camera->position);
    	mandelbrot.uploadVec2("uProjectionSize", this->camera->getProjectionSize() / this->camera->getZoom());
		glClearColor(0.015625f, 0.015625f, 0.015625f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);
		renderer.render();
		framebuffer.unbind();

		// Apply post processing to the image.
		glActiveTexture(GL_TEXTURE0 + 1);
		framebuffer.getTexture()->bind();
		hsv.uploadTexture("uTexture", 0);
		framebuffer.getTexture()->unbind();
		renderer.render();

		MouseListener::endFrame();
		glfwSwapBuffers(this->glfwWindow);

	}

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