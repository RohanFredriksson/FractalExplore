#include <iostream>
#include <stdlib.h>

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

#include "camera.hpp"
#include "shader.hpp"
#include "window.hpp"

Window::Window() {

    this->width = 1920;
    this->height = 1080;
    this->fps = -1.0f;

}

Window* Window::get() {

    if (Window::window == NULL) {
        Window::window = new Window;
    }

    return Window::window;

}

void Window::init() {

    // Initialize GLFW
	if (!glfwInit()) {
		std::cout << "Unable to initialise GLFW." << std::endl;
		return;
	}

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	// Create a GLFWwindow object of 800 by 800 pixels, naming it "YoutubeOpenGL"
	this->glfwWindow = glfwCreateWindow(800, 800, "Fractal Explore", NULL, NULL);
	// Error check if the window fails to create
	if (this->glfwWindow == NULL) {
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return;
	}
	// Introduce the window into the current context
	glfwMakeContextCurrent(this->glfwWindow);

	//Load GLAD so it configures OpenGL
	gladLoadGL();
	// Specify the viewport of OpenGL in the Window
	// In this case the viewport goes from x = 0, y = 0, to x = 800, y = 800
	glViewport(0, 0, 800, 800);

}

void Window::loop() {

	Camera camera(glm::vec2(-2.0f, -2.0f));

    const char* v = "assets/shaders/mandelbrot.vert";
	const char* f = "assets/shaders/mandelbrot.frag";
	Shader test(v, f);
	test.compile();
	test.uploadMat4("uProjection", camera.getProjection());
	test.uploadMat4("uView", camera.getView());

	// Vertices coordinates
	GLfloat vertices[] =
	{
		 2.0f,  2.0f, // Top Right
		 2.0f, -2.0f, // Bottom Right
		-2.0f, -2.0f, // Bottom Left
		-2.0f,  2.0f, // Top Left
	};

	// Indices for vertices order
	GLuint indices[] =
	{
		3, 2, 0,
		0, 2, 1,
	};

	// Create reference containers for the Vartex Array Object, the Vertex Buffer Object, and the Element Buffer Object
	GLuint VAO, VBO, EBO;

	// Generate the VAO, VBO, and EBO with only 1 object each
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);

	// Make the VAO the current Vertex Array Object by binding it
	glBindVertexArray(VAO);

	// Bind the VBO specifying it's a GL_ARRAY_BUFFER
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	// Introduce the vertices into the VBO
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	// Bind the EBO specifying it's a GL_ELEMENT_ARRAY_BUFFER
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	// Introduce the indices into the EBO
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	// Configure the Vertex Attribute so that OpenGL knows how to read the VBO
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
	// Enable the Vertex Attribute so that OpenGL knows to use it
	glEnableVertexAttribArray(0);

	// Bind both the VBO and VAO to 0 so that we don't accidentally modify the VAO and VBO we created
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
	// Bind the EBO to 0 so that we don't accidentally modify it
	// MAKE SURE TO UNBIND IT AFTER UNBINDING THE VAO, as the EBO is linked in the VAO
	// This does not apply to the VBO because the VBO is already linked to the VAO during glVertexAttribPointer
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	// Main while loop
	while (!glfwWindowShouldClose(this->glfwWindow))
	{
		// Specify the color of the background
		glClearColor(0.07f, 0.13f, 0.17f, 1.0f);
		// Clean the back buffer and assign the new color to it
		glClear(GL_COLOR_BUFFER_BIT);
		// Tell OpenGL which Shader Program we want to use
		test.use();
		// Bind the VAO so OpenGL knows to use it
		glBindVertexArray(VAO);
		// Draw primitives, number of indices, datatype of indices, index of indices
		glDrawElements(GL_TRIANGLES, sizeof(indices) / sizeof(GLuint), GL_UNSIGNED_INT, 0);    // CHANGE THE NUMBER OF LINES
		// Swap the back buffer with the front buffer
		glfwSwapBuffers(this->glfwWindow);
		// Take care of all GLFW events
		glfwPollEvents();
	}

    // Delete all the objects we've created
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	glDeleteBuffers(1, &EBO);

    test.deleteProgram();
    
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

Window* Window::window = NULL;