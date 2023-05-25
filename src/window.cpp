#include <iostream>
#include <cstdlib>

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

#include "window.hpp"
#include "arbitrary.hpp"
#include "camera.hpp"
#include "shader.hpp"
#include "listener.hpp"
#include "framebuffer.hpp"
#include "renderer.hpp"

namespace {

	GLFWwindow* window;
	Camera camera;
	Framebuffer* postprocessing;

	int width = 800;
	int height = 800;
	float fps = -1.0f;

}

namespace Window {

	int getWidth() {
		return width;
	}

	int getHeight() {
		return height;
	}

	float getAspectRatio() {
		return (float) width / (float) height;
	}

	Camera* getCamera() {
		return &camera;
	}

	void setWidth(int w) {
		width = w;
	}

	void setHeight(int h) {
		height = h;
	}

	void resetFramebuffers() {
		delete postprocessing;
		postprocessing = new Framebuffer(GL_RGB, width, height, GL_RGB, GL_UNSIGNED_BYTE);
	}

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
	glfwSetWindowSizeCallback(window, WindowListener::resizeCallback);
	glfwSetCursorPosCallback(window, MouseListener::mousePosCallback);
	glfwSetMouseButtonCallback(window, MouseListener::mouseButtonCallback);
	glfwSetScrollCallback(window, MouseListener::mouseScrollCallback);

	// Make the OpenGl context current
	glfwMakeContextCurrent(window);

	//Enable v-sync
    glfwSwapInterval(1);

	//Load GLAD so it configures OpenGL
	gladLoadGL();

	// Framebuffer
	postprocessing = new Framebuffer(GL_RGB, width, height, GL_RGB, GL_UNSIGNED_BYTE);

	// Shaders
	Shader mandelbrot("assets/shaders/mandelbrot_arbitrary.vert", "assets/shaders/mandelbrot_arbitrary.frag");
	Shader hsv("assets/shaders/hsv.vert", "assets/shaders/hsv.frag");

	// Renderer
	Renderer renderer;
	renderer.start();

	// Call the resize callback for initialisation.
	WindowListener::resizeCallback(window, width, height);
	
	// Main while loop
	while (!glfwWindowShouldClose(window)) {	

		glfwPollEvents();
		
		if (MouseListener::isMouseDragging()) {
			if (MouseListener::getDx() != 0.0) {camera.x = camera.x - (MouseListener::getWorldDx());}
			if (MouseListener::getDy() != 0.0) {camera.y = camera.y + (MouseListener::getWorldDy());}
		}

		if (MouseListener::getScrollY() != 0.0f) {

			if (MouseListener::getScrollY() > 0) {
				camera.depth = camera.depth * Arbitrary(1.0f / 1.1f);
				camera.x = camera.x + (MouseListener::getWorldX() - camera.x) * Arbitrary(0.0909090909f);
				camera.y = camera.y - (MouseListener::getWorldY() - camera.y) * Arbitrary(0.0909090909f);
			}

			else {
				camera.depth = camera.depth * (Arbitrary(1.1f));
				camera.x = camera.x - (MouseListener::getWorldX() - camera.x) * Arbitrary(0.1f);
				camera.y = camera.y + (MouseListener::getWorldY() - camera.y) * Arbitrary(0.1f);
			}

			camera.adjust();
			
		}

		// Render grayscale mandelbrot set to framebuffer.
		postprocessing->bind();
		mandelbrot.uploadIntArray("ux", Arbitrary::precision(), camera.x.data());
		mandelbrot.uploadIntArray("uy", Arbitrary::precision(), camera.y.data());
		mandelbrot.uploadIntArray("uw", Arbitrary::precision(), camera.width.data());
		mandelbrot.uploadIntArray("uh", Arbitrary::precision(), camera.height.data());
		mandelbrot.uploadIntArray("ud", Arbitrary::precision(), camera.depth.data());
		glClearColor(0.015625f, 0.015625f, 0.015625f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);
		renderer.render();
		postprocessing->unbind();

		// Apply post processing to the image.
		hsv.uploadTexture("uTexture", 0);
		renderer.render();

		MouseListener::endFrame();
		glfwSwapBuffers(window);

	}

	// Destroy the framebuffer.
	delete postprocessing;

	// Destroy the window and GLFW.
	glfwDestroyWindow(window);
	glfwTerminate();

	return 0;
}