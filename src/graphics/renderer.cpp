#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "graphics/renderer.hpp"

const int POS_SIZE = 2;
const int TEX_COORDS_SIZE = 2;

const int POS_OFFSET = 0;
const int TEX_COORDS_OFFSET = POS_OFFSET + POS_SIZE * sizeof(float);

const int VERTEX_SIZE = POS_SIZE + TEX_COORDS_SIZE;
const int VERTEX_SIZE_BYTES = VERTEX_SIZE * sizeof(float);

GLfloat vertices[] = {
     1.0f,  1.0f, 1.0f, 1.0f, // Top Right
     1.0f, -1.0f, 1.0f, 0.0f, // Bottom Right
    -1.0f, -1.0f, 0.0f, 0.0f, // Bottom Left 
    -1.0f,  1.0f, 0.0f, 1.0f  // Top Left
};

GLuint indices[] = {
    3, 2, 0,
    0, 2, 1,
};

void Renderer::start() {

	glGenVertexArrays(1, &this->VAO);
	glGenBuffers(1, &this->VBO);
	glGenBuffers(1, &this->EBO);

	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, this->VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, POS_SIZE, GL_FLOAT, GL_FALSE, VERTEX_SIZE_BYTES, (void*) POS_OFFSET);
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, TEX_COORDS_SIZE, GL_FLOAT, GL_FALSE, VERTEX_SIZE_BYTES, (void*) TEX_COORDS_OFFSET);
    glEnableVertexAttribArray(1);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

}

void Renderer::render() {
    glBindVertexArray(this->VAO);
    glDrawElements(GL_TRIANGLES, sizeof(indices) / sizeof(GLuint), GL_UNSIGNED_INT, 0);
}