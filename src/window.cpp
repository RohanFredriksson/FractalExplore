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
	Shader mandelbrot("assets/shaders/mandelbrot.vert", "assets/shaders/mandelbrot.frag");
	Shader hsv("assets/shaders/hsv.vert", "assets/shaders/hsv.frag");

	// Renderer
	Renderer renderer;
	renderer.start();

	// Call the resize callback for initialisation.
	WindowListener::resizeCallback(window, width, height);

	// Loop variables
	bool update = true;
	float begin = (float) glfwGetTime();
	float end = begin;
	float dt = -1.0f;

	// Main while loop
	while (!glfwWindowShouldClose(window)) {	

		// Polling Stage
		glfwPollEvents();

		// Update Stage
		if (MouseListener::isMouseDragging()) {
			if (MouseListener::getDx() != 0.0) {camera.x = camera.x - (MouseListener::getWorldDx());}
			if (MouseListener::getDy() != 0.0) {camera.y = camera.y + (MouseListener::getWorldDy());}
			update = true;
		}

		if (MouseListener::getScrollY() != 0.0f) {

			if (MouseListener::getScrollY() > 0) {
				camera.depth = camera.depth * Arbitrary(1.0 / 1.1);
				camera.x = camera.x + (MouseListener::getWorldX() - camera.x) * Arbitrary(0.0909090909f);
				camera.y = camera.y - (MouseListener::getWorldY() - camera.y) * Arbitrary(0.0909090909f);
			} else {
				camera.depth = camera.depth * (Arbitrary(1.1));
				camera.x = camera.x - (MouseListener::getWorldX() - camera.x) * Arbitrary(0.1f);
				camera.y = camera.y + (MouseListener::getWorldY() - camera.y) * Arbitrary(0.1f);
			}

			//for (int i = 1; i <= Arbitrary::precision(); i++) {std::cout << camera.depth.values[i] << " ";} std::cout << "\n";
			update = true;
		}

		// Render Stage
		if (update) {

			int iterations = 256;

			// Render the mandelbrot in black and white.
			postprocessing->bind();
			mandelbrot.uploadInt("uIterations", iterations);
			mandelbrot.uploadUnsignedIntArray("uReal", Arbitrary::precision()+1, camera.x.data());
			mandelbrot.uploadUnsignedIntArray("uImaginary", Arbitrary::precision()+1, camera.y.data());
			mandelbrot.uploadUnsignedIntArray("uWidth", Arbitrary::precision()+1, camera.width.data());
			mandelbrot.uploadUnsignedIntArray("uHeight", Arbitrary::precision()+1, camera.height.data());
			mandelbrot.uploadUnsignedIntArray("uDepth", Arbitrary::precision()+1, camera.depth.data());
			glClearColor(0.015625f, 0.015625f, 0.015625f, 1.0f);
			glClear(GL_COLOR_BUFFER_BIT);
			renderer.render();
			postprocessing->unbind();

			// Apply post processing to the image.
			hsv.uploadTexture("uTexture", 0);
			renderer.render();

			// Swap the buffer and lower the update flag.
			glfwSwapBuffers(window);
			update = false;

		}
		
		// End Frame
		MouseListener::endFrame();
		end = (float) glfwGetTime();
		dt = end - begin;
		begin = end;
		fps = 1.0f / dt;

	}

	// Destroy the framebuffer.
	delete postprocessing;

	// Destroy the window and GLFW.
	glfwDestroyWindow(window);
	glfwTerminate();

	return 0;
}