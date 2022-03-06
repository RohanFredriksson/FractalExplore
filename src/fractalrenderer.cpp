#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "fractalrenderer.hpp"
#include "window.hpp"

GLfloat vertices[] =
{
    2.0f,  2.0f, // Top Right
    2.0f, -2.0f, // Bottom Right
    -2.0f, -2.0f, // Bottom Left
    -2.0f,  2.0f, // Top Left
};

GLuint indices[] =
{
    3, 2, 0,
    0, 2, 1,
};

FractalRenderer::FractalRenderer(Shader* shader) {
    this->shader = shader;
}

void FractalRenderer::start() {

    // Generate the VAO, VBO, and EBO with only 1 object each
	glGenVertexArrays(1, &this->VAO);
	glGenBuffers(1, &this->VBO);
	glGenBuffers(1, &this->EBO);

	// Make the VAO the current Vertex Array Object by binding it
	glBindVertexArray(VAO);

	// Bind the VBO specifying it's a GL_ARRAY_BUFFER
	glBindBuffer(GL_ARRAY_BUFFER, this->VBO);
	// Introduce the vertices into the VBO
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	// Bind the EBO specifying it's a GL_ELEMENT_ARRAY_BUFFER
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->EBO);
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

}

void FractalRenderer::render() {

    this->shader->uploadMat4("uProjection", Window::getCamera()->getProjection());
    this->shader->uploadMat4("uView", Window::getCamera()->getView());

    glBindVertexArray(this->VAO);
    glDrawElements(GL_TRIANGLES, sizeof(indices) / sizeof(GLuint), GL_UNSIGNED_INT, 0);

}