#include <iostream>
#include <cstdlib>
#include <string>

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb/stb_image_write.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/ext.hpp>

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

#include "arbitrary.hpp"
#include "shader.hpp"
#include "framebuffer.hpp"
#include "renderer.hpp"

#include "shaders/hsv.hpp"
#include "fractal.hpp"

class Camera {

    public:

        Arbitrary x;
        Arbitrary y;
        Arbitrary width;
        Arbitrary height;
        Arbitrary depth;

        Camera();
        void adjust();

};

namespace KeyListener {

    void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
    bool isKeyDown(int key);
    bool isKeyDown();
    bool isKeyBeginDown(int key);

}

namespace MouseListener {

    void calcOrthoX();
    void calcOrthoY();
    void mousePosCallback(GLFWwindow* window, double xPos, double yPos);
    void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods);
    void mouseScrollCallback(GLFWwindow* window, double xOffset, double yOffset);
    bool isMouseDown(int button);
    bool isMouseDown();
    bool isMouseBeginDown(int button);
    bool hasMouseMoved();
    bool isMouseDragging();
    double getX();
    double getY();
    double getDx();
    double getDy();
    Arbitrary getWorldX();
    Arbitrary getWorldY();
    Arbitrary getWorldDx();
    Arbitrary getWorldDy();
    double getScrollX();
    double getScrollY();
    void endFrame();

}

namespace {

	const int max = Arbitrary::precision();

	GLFWwindow* window;
	Camera camera;
	Framebuffer* renderbuffer = nullptr;
	Framebuffer* savebuffer = nullptr;

	Shader* fractal = nullptr;
	Shader* postprocessing = nullptr;

	int precision = 2;
	int iterations = 64;
	int downsampling = 1;

	bool update = true;

	int width = 800;
	int height = 800;
	float fps = -1.0f;

	float ratio() {
		return (float) width / (float) height;
	}

}

Camera::Camera() {
    this->x = Arbitrary(0.0);
    this->y = Arbitrary(0.0);
    this->width = Arbitrary(2.0);
    this->height = Arbitrary(2.0);
    this->depth = Arbitrary(1.0);
    this->adjust();
}

void Camera::adjust() {
    this->width = this->height * Arbitrary(ratio());
}

void resize(GLFWwindow* window, int screenWidth, int screenHeight) {

	width = screenWidth;
	height = screenHeight;

	if (renderbuffer != nullptr) {delete renderbuffer;}
	renderbuffer = new Framebuffer(GL_RGB, width, height, GL_RGB, GL_UNSIGNED_BYTE);

	if (savebuffer != nullptr) {delete savebuffer;}
	savebuffer = new Framebuffer(GL_RGB, width, height, GL_RGB, GL_UNSIGNED_BYTE);

	camera.adjust();
	glViewport(0, 0, screenWidth, screenHeight);
	update = true;

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
	glfwSetWindowSizeCallback(window, resize);
	glfwSetKeyCallback(window, KeyListener::keyCallback);
	glfwSetCursorPosCallback(window, MouseListener::mousePosCallback);
	glfwSetMouseButtonCallback(window, MouseListener::mouseButtonCallback);
	glfwSetScrollCallback(window, MouseListener::mouseScrollCallback);

	// Make the OpenGl context current
	glfwMakeContextCurrent(window);

	//Enable v-sync
    glfwSwapInterval(1);

	//Load GLAD so it configures OpenGL
	gladLoadGL();

	// Initialise ImGui
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	(void)io;
	ImGui::StyleColorsDark();
	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init("#version 330");

	// Initialise the shaders.
	fractal = FRACTAL_FACTORY.create("Mandelbrot", precision);
	Shader hsv(HSV::vertex, HSV::fragment);

	// Renderer
	Renderer renderer;
	renderer.start();

	// Call the resize callback for initialisation.
	resize(window, width, height);

	// Loop variables
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
			if (MouseListener::getDy() != 0.0) {camera.y = camera.y - (MouseListener::getWorldDy());}
			update = true;
		}

		if (MouseListener::getScrollY() != 0.0f) {

			if (MouseListener::getScrollY() > 0) {
				camera.depth = camera.depth * Arbitrary(1.0 / 1.1);
				camera.x = camera.x + (MouseListener::getWorldX() - camera.x) * Arbitrary(0.0909090909f);
				camera.y = camera.y + (MouseListener::getWorldY() - camera.y) * Arbitrary(0.0909090909f);
			} else {
				camera.depth = camera.depth * (Arbitrary(1.1));
				camera.x = camera.x - (MouseListener::getWorldX() - camera.x) * Arbitrary(0.1f);
				camera.y = camera.y - (MouseListener::getWorldY() - camera.y) * Arbitrary(0.1f);
			}

			update = true;
		}

		// Render Stage
		if (update) {

			update = false;

			glViewport(0, 0, width, height);

			// Determine the scale factor for the shader.
			Arbitrary w = Arbitrary(0.5f) * camera.depth * camera.width;
			Arbitrary h = Arbitrary(0.5f) * camera.depth * camera.height;

			// Render the mandelbrot in black and white to the buffer.
			renderbuffer->bind();
			fractal->uploadInt("uIterations", iterations);
			fractal->uploadUnsignedIntArray("uPositionX", Arbitrary::precision()+1, camera.x.data());
			fractal->uploadUnsignedIntArray("uPositionY", Arbitrary::precision()+1, camera.y.data());
			fractal->uploadUnsignedIntArray("uScaleX", Arbitrary::precision()+1, w.data());
			fractal->uploadUnsignedIntArray("uScaleY", Arbitrary::precision()+1, h.data());
			glClearColor(0.015625f, 0.015625f, 0.015625f, 1.0f);
			glClear(GL_COLOR_BUFFER_BIT);
			renderer.render();
			renderbuffer->unbind();

			glViewport(0, 0, width, height);

		}

		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		// Apply post processing to the image.
		renderbuffer->getTexture()->bind();
		hsv.uploadTexture("uTexture", 0);
		savebuffer->bind();   				// TODO: ONLY CALL TO THE SAVE BUFFER IF THERE IS A SAVE EVENT.
		renderer.render();    				// 
		savebuffer->unbind(); 				//
		renderer.render();
		renderbuffer->getTexture()->unbind();

		// Imgui
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		// Main menu bar
        if (ImGui::BeginMainMenuBar()) {
            if (ImGui::BeginMenu("File")) {
                if (ImGui::MenuItem("Save", "Ctrl+S")) {}
                if (ImGui::MenuItem("Quit", "Alt+F4")) { glfwSetWindowShouldClose(window, true); }
                ImGui::EndMenu();
            }

            if (ImGui::BeginMenu("Edit")) {
                if (ImGui::MenuItem("Cut", "Ctrl+X")) {}
                if (ImGui::MenuItem("Copy", "Ctrl+C")) {}
                if (ImGui::MenuItem("Paste", "Ctrl+V")) {}
                ImGui::EndMenu();
            }

			if (ImGui::BeginMenu("Fractal")) {
                if (ImGui::MenuItem("Position", "Ctrl+X")) {}
                if (ImGui::MenuItem("Depth", "Ctrl+C")) {}
                if (ImGui::MenuItem("Type", "Ctrl+V")) {}
                ImGui::EndMenu();
            }

			if (ImGui::BeginMenu("Color")) {
                if (ImGui::MenuItem("Cut", "Ctrl+X")) {}
                if (ImGui::MenuItem("Copy", "Ctrl+C")) {}
                if (ImGui::MenuItem("Paste", "Ctrl+V")) {}
                ImGui::EndMenu();
            }

            // Add more menus or buttons here if needed

            ImGui::EndMainMenuBar();
        }

		ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
		glfwSwapBuffers(window);
		
		// End Frame
		MouseListener::endFrame();
		end = (float) glfwGetTime();
		dt = end - begin;
		begin = end;
		fps = 1.0f / dt;

	}

	// For now just to test, we will save the save buffer on close.
	// TODO: create a save event to call these lines of code.
	unsigned char* data = (unsigned char*) malloc(width * height * 3);
	savebuffer->bind();
	glReadPixels(0, 0, width, height, GL_RGB, GL_UNSIGNED_BYTE, data);
	savebuffer->unbind();
	stbi_write_png("test.png", width, height, 3, data, width * 3);
	free(data);

	// Destroy the framebuffers.
	delete renderbuffer;
	delete savebuffer;

	// Destroy the shaders.
	delete fractal;

	// Destroy imgui
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();
	glfwDestroyWindow(window);
	glfwTerminate();

	// Destroy the window and GLFW.
	glfwDestroyWindow(window);
	glfwTerminate();

	return 0;
}

namespace {

    bool keyDown[350] = {0};
    bool keyBeginDown[350] = {0};
    int keyPressCount = 0;

    double scrollX = 0.0;
    double scrollY = 0.0;
    double x = 0.0; 
    double y = 0.0; 
    double lastX = 0.0; 
    double lastY = 0.0; 
    Arbitrary worldX(0.0f); 
    Arbitrary worldY (0.0f); 
    Arbitrary lastWorldX(0.0f); 
    Arbitrary lastWorldY(0.0f);
    bool mouseDown[9] = {0};
    bool mouseBeginDown[9] = {0};
    int mouseDownCount = 0;
    bool mouseDragging = 0;
    
}

void KeyListener::keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {

    if (key < 0 || key > 349) {return;}

    if (action == GLFW_PRESS) {
        keyPressCount++;
        keyDown[key] = true;
        keyBeginDown[key] = true;
    }

    else if (action == GLFW_RELEASE) {
        keyPressCount--;
        keyDown[key] = false;
        keyBeginDown[key] = false;
    }

}

bool KeyListener::isKeyDown(int key) {
    if (key < 0 || key > 349) {return false;}
    return keyDown[key];
}

bool KeyListener::isKeyDown() {
    return keyPressCount > 0;
}

bool KeyListener::isKeyBeginDown(int key) {

    if (key < 0 || key > 349) {return false;}

    if (keyBeginDown[key]) {
        keyBeginDown[key] = false;
        return true;
    }

    return false;
}

void MouseListener::calcOrthoX() {
    float currentX = ((float) x / width) * 2.0f - 1.0f;
    worldX = camera.x + Arbitrary(0.5f * currentX) * (camera.width * camera.depth);
}

void MouseListener::calcOrthoY() {
    float currentY = ((float) y / height) * 2.0f - 1.0f;
    worldY = camera.y + Arbitrary(0.5f * currentY) * (camera.height * camera.depth);
}

void MouseListener::mousePosCallback(GLFWwindow* window, double xPos, double yPos) {

    if (mouseDownCount > 0) {
        mouseDragging = true;
    }

    lastX = x;
    lastY = y;
    lastWorldX = worldX;
    lastWorldY = worldY;
    x = xPos;
    y = yPos;
    calcOrthoX();
    calcOrthoY();

}

void MouseListener::mouseButtonCallback(GLFWwindow* window, int button, int action, int mods) {

    if (button > 8 || button < 0) {return;}

    if (action == GLFW_PRESS) {
        mouseDownCount++;
        mouseDown[button] = true;
        mouseBeginDown[button] = true;
    }

    else if (action == GLFW_RELEASE) {
        mouseDownCount--;
        mouseDown[button] = false;
        mouseBeginDown[button] = false;
        mouseDragging = false;
    }

}

void MouseListener::mouseScrollCallback(GLFWwindow* window, double xOffset, double yOffset) {
    scrollX = xOffset;
    scrollY = yOffset;
}

bool MouseListener::isMouseDown(int button) {
    if (button < 0 || button > 9) {return false;}
    return mouseDown[button];
}

bool MouseListener::isMouseDown() {
    return mouseDownCount > 0;
}

bool MouseListener::isMouseBeginDown(int button) {

    if (button < 0 || button > 8) {return false;}

    if (mouseBeginDown[button]) {
        mouseBeginDown[button] = false;
        return true;
    }

    return false;
}

bool MouseListener::hasMouseMoved() {
    return x != lastX || y != lastY;
}

bool MouseListener::isMouseDragging() {
    return mouseDragging;
}

double MouseListener::getX() {
    return x;
}

double MouseListener::getY() {
    return y;
}

double MouseListener::getDx() {
    return x - lastX;
}

double MouseListener::getDy() {
    return y - lastY;
}

Arbitrary MouseListener::getWorldX() {
    return worldX;
}

Arbitrary MouseListener::getWorldY() {
    return worldY;
}

Arbitrary MouseListener::getWorldDx() {
    return worldX - lastWorldX;
}

Arbitrary MouseListener::getWorldDy() {
    return worldY - lastWorldY;
}

double MouseListener::getScrollX() {
    return scrollX;
}

double MouseListener::getScrollY() {
    return scrollY;
}

void MouseListener::endFrame() {

    scrollX = 0.0;
    scrollY = 0.0;
    lastX = x;
    lastY = y;
    lastWorldX = worldX;
    lastWorldY = worldY;
    calcOrthoX();
    calcOrthoY();

}