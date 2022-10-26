#include "external.h"
#include "renderer.h"
#include "camera.h"
#include "util.h"

Shader* shader;
GLuint VAO, VBO, EBO;

GLfloat vertices[] = {
    1.0f,  1.0f, // Top Right
    1.0f, -1.0f, // Bottom Right
    -1.0f, -1.0f, // Bottom Left
    -1.0f,  1.0f, // Top Left
};

GLuint indices[] = {
    3, 2, 0,
    0, 2, 1,
};

void Renderer_Init() {

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

}

void Renderer_BindShader(Shader* s) {
    shader = s;
}

void Renderer_Render() {
    
	double xMin = Camera_GetX() - Camera_GetProjectionWidth() / 2.0;
	double yMin = Camera_GetY() - Camera_GetProjectionHeight() / 2.0;
	double xMax = Camera_GetX() + Camera_GetProjectionWidth() / 2.0;
	double yMax = Camera_GetY() + Camera_GetProjectionHeight() / 2.0;

	float xMinHigh, xMinLow, yMinHigh, yMinLow, xMaxHigh, xMaxLow, yMaxHigh, yMaxLow;
	split_double(xMin, &xMinHigh, &xMinLow);
	split_double(xMax, &xMaxHigh, &xMaxLow);
	split_double(yMin, &yMinHigh, &yMinLow);
	split_double(yMax, &yMaxHigh, &yMaxLow);

	Shader_UploadFloat(shader, "xMinHigh", xMinHigh);
	Shader_UploadFloat(shader, "xMinLow",  xMinLow);
	Shader_UploadFloat(shader, "yMinHigh", yMinHigh);
	Shader_UploadFloat(shader, "yMinLow",  yMinLow);
	Shader_UploadFloat(shader, "xMaxHigh", xMaxHigh);
	Shader_UploadFloat(shader, "xMaxLow",  xMaxLow);
	Shader_UploadFloat(shader, "yMaxHigh", yMaxHigh);
	Shader_UploadFloat(shader, "yMaxLow",  yMaxLow);

    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, sizeof(indices) / sizeof(GLuint), GL_UNSIGNED_INT, 0);

}