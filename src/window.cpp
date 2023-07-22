#include <iostream>
#include <cstdlib>

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

#include "window.hpp"
#include "arbitrary.hpp"
#include "camera.hpp"
#include "shader.hpp"
#include "listener.hpp"
#include "framebuffer.hpp"
#include "renderer.hpp"

#include "shaders/mandelbrot.hpp"
#include "shaders/hsv.hpp"

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

	Arbitrary a(3.14159265358979323846264338327950288);
	std::string p = Arbitrary::serialise(a);
	std::cout << p << "\n";

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

	// Initialise ImGui
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	(void)io;
	ImGui::StyleColorsDark();
	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init("#version 330");

	// Framebuffer
	postprocessing = new Framebuffer(GL_RGB, width, height, GL_RGB, GL_UNSIGNED_BYTE);
	
	Shader mandelbrot(Mandelbrot::vertex, Mandelbrot::fragment);
	Shader hsv(HSV::vertex, HSV::fragment);

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

			update = true;
		}

		/*
		// Render Stage
		if (update) {

			int iterations = 256;

			// Determine the scale factor for the shader.
			Arbitrary w = Arbitrary(0.5f) * camera.depth * camera.width;
			Arbitrary h = Arbitrary(0.5f) * camera.depth * camera.height;

			// Render the mandelbrot in black and white.
			postprocessing->bind();
			mandelbrot.uploadInt("uIterations", iterations);
			mandelbrot.uploadUnsignedIntArray("uPositionX", Arbitrary::precision()+1, camera.x.data());
			mandelbrot.uploadUnsignedIntArray("uPositionY", Arbitrary::precision()+1, camera.y.data());
			mandelbrot.uploadUnsignedIntArray("uScaleX", Arbitrary::precision()+1, w.data());
			mandelbrot.uploadUnsignedIntArray("uScaleY", Arbitrary::precision()+1, h.data());
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
		*/

		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		ImGui::SetCursorPosY(-30.0f);
    	ImGui::Text("Custom Header Content");

		// Main menu bar
        if (ImGui::BeginMainMenuBar()) {
            if (ImGui::BeginMenu("File")) {
                // Add menu items for File menu
                if (ImGui::MenuItem("Open", "Ctrl+O")) {}
                if (ImGui::MenuItem("Save", "Ctrl+S")) {}
                if (ImGui::MenuItem("Quit", "Alt+F4")) { glfwSetWindowShouldClose(window, true); }
                ImGui::EndMenu();
            }

            if (ImGui::BeginMenu("Edit")) {
                // Add menu items for Edit menu
                if (ImGui::MenuItem("Cut", "Ctrl+X")) {}
                if (ImGui::MenuItem("Copy", "Ctrl+C")) {}
                if (ImGui::MenuItem("Paste", "Ctrl+V")) {}
                ImGui::EndMenu();
            }

            // Add more menus or buttons here if needed

            ImGui::EndMainMenuBar();
        }

        static bool openRightWindow = false;

		// Main window content
		//ImGui::SetNextWindowSize(ImVec2(300, 200));
		//ImGui::SetNextWindowPos(ImVec2(0, 20));
		ImGui::Begin("DockSpace Window");
		ImGui::Text("Main Window Content");
		if (ImGui::Button("Open Right Window")) {openRightWindow = true;}
		ImGui::End();

		if (openRightWindow) {

			//ImGui::SetNextWindowSize(ImVec2(300, 200));
			//ImGui::SetNextWindowPos(ImVec2(800 - 300, 0));
			ImGui::Begin("Secondary Window", &openRightWindow);
			ImGui::Text("Secondary Window Content");
			ImGui::End();

        }

		glClearColor(0.45f, 0.55f, 0.60f, 1.00f);
        glClear(GL_COLOR_BUFFER_BIT);
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

	// Destroy the framebuffer.
	delete postprocessing;

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