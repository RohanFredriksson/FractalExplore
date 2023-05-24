#include <iostream>
#include <stdlib.h>

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

#include "arbitrary.hpp"
#include "camera.hpp"
#include "shader.hpp"
#include "listener.hpp"
#include "framebuffer.hpp"
#include "renderer.hpp"

namespace {

	GLFWwindow* window;
	Camera camera;
	int width = 800;
	int height = 800;
	float fps;

}

int main() {

	// Initialise GLFW
	if (!glfwInit()) {
		std::cout << "Unable to initialise GLFW." << std::endl;
		return 1;
	}

	// Configure GLFW
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	// Create the window
	window = glfwCreateWindow(width, height, "Fractal Explore", NULL, NULL);
	if (window == NULL) {
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return 1;
	}

	// Manage callbacks
	glfwSetKeyCallback(window, KeyListener::keyCallback);
	glfwSetWindowSizeCallback(window, WindowResizeListener::resizeCallback);
	glfwSetCursorPosCallback(window, MouseListener::mousePosCallback);
	glfwSetMouseButtonCallback(window, MouseListener::mouseButtonCallback);
	glfwSetScrollCallback(window, MouseListener::mouseScrollCallback);

	// Make the OpenGl context current
	glfwMakeContextCurrent(window);

	//Enable v-sync
    glfwSwapInterval(1);

	//Load GLAD so it configures OpenGL
	gladLoadGL();

	// Call the resize callback for initialisation.
	WindowResizeListener::resizeCallback(window, width, height);

	// Shaders
	Shader mandelbrot("assets/shaders/mandelbrot_arbitrary.vert", "assets/shaders/mandelbrot_arbitrary.frag");
	Shader hsv("assets/shaders/hsv.vert", "assets/shaders/hsv.frag");

	// Framebuffer
	Framebuffer framebuffer(GL_RGB, width, height, GL_RGB, GL_UNSIGNED_BYTE);

	// Renderer
	Renderer renderer;
	renderer.start();
	
	// Main while loop
	while (!glfwWindowShouldClose(window)) {	

		glfwPollEvents();
		
		//if (MouseListener::isDragging()) {
		//	if (MouseListener::getDx() != 0) {this->camera->position.x = this->camera->position.x + (MouseListener::getWorldDx());}
		//	if (MouseListener::getDy() != 0) {this->camera->position.y = this->camera->position.y - (MouseListener::getWorldDy());}
		//}

		if (MouseListener::getScrollY() != 0.0f) {

			if (MouseListener::getScrollY() > 0) {
				//camera.zoom = camera.zoom * Arbitrary(1.1f);
				//this->camera->position.x = this->camera->position.x + (MouseListener::getOrthoX() - this->camera->position.x) * 0.0909090909f;
				//this->camera->position.y = this->camera->position.y - (MouseListener::getOrthoY() - this->camera->position.y) * 0.0909090909f;
			}

			else {
				//this->camera->setZoom(this->camera->getZoom() / 1.1f);
				//this->camera->position.x = this->camera->position.x - (MouseListener::getOrthoX() - this->camera->position.x) * 0.1f;
				//this->camera->position.y = this->camera->position.y + (MouseListener::getOrthoY() - this->camera->position.y) * 0.1f;
			}

			camera.adjust();
			
		}

		// Render grayscale mandelbrot set to framebuffer.
		framebuffer.bind();
		mandelbrot.uploadIntArray("ux", camera.x.precision(), camera.x.values);
		mandelbrot.uploadIntArray("uy", camera.x.precision(), camera.y.values);
		mandelbrot.uploadIntArray("uw", camera.x.precision(), camera.width.values);
		mandelbrot.uploadIntArray("uh", camera.x.precision(), camera.height.values);
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
		glfwSwapBuffers(window);

	}

	glfwDestroyWindow(window);
	glfwTerminate();

	return 0;
}