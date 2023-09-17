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
#include "graphics.hpp"

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

	GLFWwindow* window;
	Camera camera;
	Framebuffer* renderbuffer = nullptr;
	Framebuffer* savebuffer = nullptr;

	Shader* fractal = nullptr;
	Shader* postprocessing = nullptr;

	bool locationwindow = true;
	bool fractalwindow = false;
	int fractaloption = 0;
	int coloroption = 0;
	int precision = 1;
	int iterations = 64;
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

void resize(GLFWwindow* window, int w, int h) {

	width = w;
	height = h;

	if (renderbuffer != nullptr) {delete renderbuffer;}
	renderbuffer = new Framebuffer(GL_RGB, width, height, GL_RGB, GL_UNSIGNED_BYTE);

	if (savebuffer != nullptr) {delete savebuffer;}
	savebuffer = new Framebuffer(GL_RGB, width, height, GL_RGB, GL_UNSIGNED_BYTE);

	camera.adjust();
	glViewport(0, 0, width, height);
	update = true;

}

int main() {

	std::vector<ShaderProgram*> programs = ShaderProgramPool::get().list("Fractal");
	for (ShaderProgram* program : programs) {
		std::cout << program->getName() << "\n";
	}

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
	fractal = FRACTAL.get("Mandelbrot", precision+1);
	postprocessing = POSTPROCESSING.get("HSV");

	// Fix the imgui options in the menus.
	std::vector<std::string> fractals = FRACTAL.list();
	for (int a = 0; a < fractals.size(); a++) {if (fractals[a] == "Mandelbrot") {fractaloption = a; break;}}
	std::vector<std::string> colors = POSTPROCESSING.list();
	for (int a = 0; a < colors.size(); a++) {if (colors[a] == "HSV") {coloroption = a; break;}}

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
			if (MouseListener::getDx() != 0.0) {camera.x = camera.x - MouseListener::getWorldDx();}
			if (MouseListener::getDy() != 0.0) {camera.y = camera.y - MouseListener::getWorldDy();}
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
			fractal->uploadUnsignedIntArray("uPositionY", Arbitrary::precision()+1, Arbitrary::negate(camera.y).data());
			fractal->uploadUnsignedIntArray("uScaleX", Arbitrary::precision()+1, w.data());
			fractal->uploadUnsignedIntArray("uScaleY", Arbitrary::precision()+1, h.data());
			renderer.render();
			renderbuffer->unbind();

			glViewport(0, 0, width, height);

		}

		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		// Apply post processing to the image.
		renderbuffer->getTexture()->bind();
		postprocessing->uploadTexture("uTexture", 0);
		savebuffer->bind();
		renderer.render();
		savebuffer->unbind();
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
                if (ImGui::MenuItem("Quit", "Alt+F4")) {glfwSetWindowShouldClose(window, true);}
                ImGui::EndMenu();
            }

            if (ImGui::BeginMenu("Configure")) {
                if (ImGui::MenuItem("Fractal", "")) {fractalwindow = !fractalwindow;}
				if (ImGui::MenuItem("Location", "")) {locationwindow = !locationwindow;}
                ImGui::EndMenu();
            }

            ImGui::EndMainMenuBar();
        }

		// Window to modify the fractal.
		if (fractalwindow) {

			ImGui::Begin("Fractal", &fractalwindow);

			fractals = FRACTAL.list();
			std::vector<const char*> strings; for (int a = 0; a < fractals.size(); a++) {strings.push_back(fractals[a].c_str());}
			if (ImGui::Combo("Fractal", &fractaloption, strings.data(), strings.size())) {
				Shader* f = FRACTAL.get(fractals[fractaloption], precision+1);
				if (f != nullptr) {
					delete fractal; 
					fractal = f;
					update = true;
				}
			}

			colors = POSTPROCESSING.list();
			strings.clear(); for (int a = 0; a < colors.size(); a++) {strings.push_back(colors[a].c_str());}
			if (ImGui::Combo("Coloring", &coloroption, strings.data(), strings.size())) {
				Shader* p = POSTPROCESSING.get(colors[coloroption]);
				if (p != nullptr) {
					delete postprocessing;
					postprocessing = p;
					update = true;
				}
			}

			int next = precision;
			ImGui::InputInt("Precision", &next);
			if (next != precision && next > 0 && next < Arbitrary::precision()) {
				precision = next;
				Shader* f = FRACTAL.get(fractals[fractaloption], precision+1);
				if (f != nullptr) {
					delete fractal; 
					fractal = f;
					update = true;
				}
			}

			next = iterations;
			ImGui::InputInt("Iterations", &next);
			if (next != iterations && next >= 0) {
				iterations = next;
				update = true;
			}

			ImGui::End();
			
		}

		if (locationwindow) {

			ImGui::Begin("Location", &locationwindow);

			int length = Arbitrary::max_length();
			char* buffer = (char*) malloc(length+1);

			std::string next = Arbitrary::serialise(camera.x);
			memcpy(buffer, next.c_str(), next.length()+1);
			ImGui::InputText("X", buffer, length);
			if (strcmp(buffer, next.c_str()) != 0) {
				
				std::string candidate(buffer);
				if (Arbitrary::validate(candidate)) {
					camera.x.load(candidate);
					update = true;
				}

			}

			next = Arbitrary::serialise(camera.y); 
			memcpy(buffer, next.c_str(), next.length()+1);
			ImGui::InputText("Y", buffer, length);
			if (strcmp(buffer, next.c_str()) != 0) {
				
				std::string candidate(buffer);
				if (Arbitrary::validate(candidate)) {
					camera.y.load(candidate);
					update = true;
				}

			}

			next = Arbitrary::serialise(camera.depth);
			memcpy(buffer, next.c_str(), next.length()+1);
			ImGui::InputText("Depth", buffer, length);
			if (strcmp(buffer, next.c_str()) != 0) {
				
				std::string candidate(buffer);
				if (Arbitrary::validate(candidate)) {
					camera.depth.load(candidate);
					update = true;
				}

			}

			free(buffer);
			ImGui::End();

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
	stbi_flip_vertically_on_write(1);
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
    worldY = camera.y + Arbitrary(-0.5f * currentY) * (camera.height * camera.depth);
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