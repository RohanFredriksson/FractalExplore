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

#include "core/arbitrary.hpp"
#include "core/window.hpp"
#include "core/listener.hpp"
#include "core/camera.hpp"
#include "graphics.hpp"

#include "programs/program.hpp"

namespace {

	GLFWwindow* window;
	Framebuffer* fractalbuffer = nullptr;
	Framebuffer* curvebuffer = nullptr;

	ShaderProgram* fractal = nullptr;
	ShaderProgram* transformation = nullptr;
	ShaderProgram* colormap = nullptr;

	bool camerawindow = false;

	bool fractalwindow = false;
	int fractaloption = 0;

	bool postprocessingwindow = false;
	int transformationoption = 0;
	int coloroption = 0;

	bool flag = true;

	int w = 800;
    int h = 800;
	float fps = -1.0f;

}

namespace Window {

	int getWidth() {
		return w;
	}

	int getHeight() {
		return h;
	}

	float getRatio() {
		return (float) w / (float) h;
	}

	void update() {
		flag = true;
	}

}

void resize(GLFWwindow* window, int width, int height) {

	w = width;
	h = height;

	if (fractalbuffer != nullptr) {delete fractalbuffer;} fractalbuffer = new Framebuffer(GL_RGB, width, height, GL_RGB, GL_UNSIGNED_BYTE);
	if   (curvebuffer != nullptr) {delete curvebuffer;}     curvebuffer = new Framebuffer(GL_RGB, width, height, GL_RGB, GL_UNSIGNED_BYTE);

	Camera::adjust();
	glViewport(0, 0, width, height);
	flag = true;

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
	window = glfwCreateWindow(Window::getWidth(), Window::getHeight(), "Fractal Explore", NULL, NULL);
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

	// Set minimum size limits
	glfwSetWindowSizeLimits(window, 400, 400, GLFW_DONT_CARE, GLFW_DONT_CARE);

	// Initialise the camera
	Camera::init();

	// Initialise ImGui
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	(void)io;
	ImGui::StyleColorsDark();
	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init("#version 330");

	// Fractal Shader Program
	fractal = ShaderProgramPool::get().get("Fractal", "Mandelbrot");
	std::vector<std::string> names = ShaderProgramPool::get().names("Fractal");
	for (int a = 0; a < names.size(); a++) {if (names[a] == "Mandelbrot") {fractaloption = a; break;}}
	
	// Transformation Shader Program
	transformation = ShaderProgramPool::get().get("Transformation", "None");
	names = ShaderProgramPool::get().names("Transformation");
	for (int a = 0; a < names.size(); a++) {if (names[a] == "None") {transformationoption = a; break;}}

	// Colormap Shader Program
	colormap = ShaderProgramPool::get().get("Colormap", "HSV");
	names = ShaderProgramPool::get().names("Colormap");
	for (int a = 0; a < names.size(); a++) {if (names[a] == "HSV") {coloroption = a; break;}}

	// Renderer
	Renderer renderer;
	renderer.start();

	// Call the resize callback for initialisation.
	resize(window, Window::getWidth(), Window::getHeight());

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
			if (MouseListener::getDx() != 0.0) {Camera::setX(Camera::getX() - MouseListener::getWorldDx());}
			if (MouseListener::getDy() != 0.0) {Camera::setY(Camera::getY() - MouseListener::getWorldDy());}
			flag = true;
		}

		if (MouseListener::getScrollY() != 0.0f) {

			if (MouseListener::getScrollY() > 0) {
				Camera::setDepth(Camera::getDepth() * Arbitrary(1.0 / 1.1));
				Camera::setX(Camera::getX() + (MouseListener::getWorldX() - Camera::getX()) * Arbitrary(0.0909090909f));
				Camera::setY(Camera::getY() + (MouseListener::getWorldY() - Camera::getY()) * Arbitrary(0.0909090909f));
			} else {
				Camera::setDepth(Camera::getDepth() * (Arbitrary(1.1)));
				Camera::setX(Camera::getX() - (MouseListener::getWorldX() - Camera::getX()) * Arbitrary(0.1f));
				Camera::setY(Camera::getY() - (MouseListener::getWorldY() - Camera::getY()) * Arbitrary(0.1f));
			}

			flag = true;
		}

		// Render Stage
		if (flag) {

			flag = false;

			glViewport(0, 0, Window::getWidth(), Window::getHeight());

			// Determine the scale factor for the shader.
			Arbitrary width = Arbitrary(0.5f) * Camera::getDepth() * Camera::getWidth();
			Arbitrary height = Arbitrary(0.5f) * Camera::getDepth() * Camera::getHeight();

			// Render the fractal in black and white to the buffer.
			fractalbuffer->bind();
			fractal->upload();
			renderer.render();
			fractalbuffer->unbind();

			glViewport(0, 0, Window::getWidth(), Window::getHeight());

		}

		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		// Apply the curve transformation to the image.
		curvebuffer->bind();
		fractalbuffer->getTexture()->bind();
		transformation->upload();
		renderer.render();
		fractalbuffer->getTexture()->unbind();
		curvebuffer->unbind();

		// Apply the color mapping to the image.
		curvebuffer->getTexture()->bind();
		colormap->upload();
		renderer.render();
		curvebuffer->getTexture()->unbind();

		// Imgui
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		// Main menu bar
		bool before = fractalwindow || camerawindow || postprocessingwindow;
        if (ImGui::BeginMainMenuBar()) {
            if (ImGui::BeginMenu("File")) {
                if (ImGui::MenuItem("Save", "Ctrl+S")) {}
                if (ImGui::MenuItem("Quit", "Alt+F4")) {glfwSetWindowShouldClose(window, true);}
                ImGui::EndMenu();
            }

            if (ImGui::BeginMenu("Configure")) {
                if (ImGui::MenuItem("Fractal", (fractalwindow ? "+": "-"))) {fractalwindow = !fractalwindow;}
				if (ImGui::MenuItem("Camera", (camerawindow ? "+": "-"))) {camerawindow = !camerawindow;}
				if (ImGui::MenuItem("PostProcessing", (postprocessingwindow ? "+": "-"))) {postprocessingwindow = !postprocessingwindow;}
                ImGui::EndMenu();
            }

            ImGui::EndMainMenuBar();
        }
		bool after = fractalwindow || camerawindow || postprocessingwindow;
		
		// Check if the window is maximised.
		int maximized = glfwGetWindowAttrib(window, GLFW_MAXIMIZED);

		// If the side window opened.
		if (!before && after && maximized == 0) {
			Arbitrary dx = Arbitrary(150) * ((Camera::getWidth() * Camera::getDepth()) / Window::getWidth());
			Camera::setX(Camera::getX() + dx);
			glfwSetWindowSize(window, Window::getWidth() + 300, Window::getHeight());
		}

		// If the side window closed.
		else if (before && !after && maximized == 0) {
			Arbitrary dx = Arbitrary(150) * ((Camera::getWidth() * Camera::getDepth()) / Window::getWidth());
			Camera::setX(Camera::getX() - dx);
			glfwSetWindowSize(window, Window::getWidth() - 300, Window::getHeight());
		}

		if (fractalwindow || camerawindow || postprocessingwindow) {

			ImGui::SetNextWindowSize(ImVec2(300, Window::getHeight()));
			ImGui::SetNextWindowPos(ImVec2(ImGui::GetIO().DisplaySize.x - 300, 18), ImGuiCond_Always);
			ImGui::Begin("#Configure", nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoTitleBar);
			
			// Fractal Properties
			ImGui::Spacing();
            if (fractalwindow && ImGui::CollapsingHeader("Fractal", ImGuiTreeNodeFlags_DefaultOpen)) {

				std::vector<std::string> names = ShaderProgramPool::get().names("Fractal");
				std::vector<const char*> strings; for (int a = 0; a < names.size(); a++) {strings.push_back(names[a].c_str());}
				if (ImGui::Combo("Type", &fractaloption, strings.data(), strings.size())) {
					ShaderProgram* p = ShaderProgramPool::get().get("Fractal", names[fractaloption]);
					if (p != nullptr) {fractal = p; flag = true;}
				}
				if (fractal != nullptr) {fractal->imgui();}

            }
			
			// Camera Properties
			ImGui::Spacing();
			if (camerawindow && ImGui::CollapsingHeader("Camera", ImGuiTreeNodeFlags_DefaultOpen)) {

				int length = Arbitrary::max_length();
				char* buffer = (char*) malloc(length+1);

				std::string next = Arbitrary::serialise(Camera::getX());
				memcpy(buffer, next.c_str(), next.length()+1);
				ImGui::InputText("X", buffer, length);
				if (strcmp(buffer, next.c_str()) != 0) {
					
					std::string candidate(buffer);
					if (Arbitrary::validate(candidate)) {
						Camera::setX(Arbitrary(candidate));
						flag = true;
					}

				}

				next = Arbitrary::serialise(Camera::getY()); 
				memcpy(buffer, next.c_str(), next.length()+1);
				ImGui::InputText("Y", buffer, length);
				if (strcmp(buffer, next.c_str()) != 0) {
					
					std::string candidate(buffer);
					if (Arbitrary::validate(candidate)) {
						Camera::setY(Arbitrary(candidate));
						flag = true;
					}

				}

				next = Arbitrary::serialise(Camera::getDepth());
				memcpy(buffer, next.c_str(), next.length()+1);
				ImGui::InputText("Depth", buffer, length);
				if (strcmp(buffer, next.c_str()) != 0) {
					
					std::string candidate(buffer);
					if (Arbitrary::validate(candidate)) {
						Camera::setDepth(Arbitrary(candidate));
						flag = true;
					}

				}

				free(buffer);
			}

			// Postprocessing Properties
			ImGui::Spacing();
			if (postprocessingwindow && ImGui::CollapsingHeader("PostProcessing", ImGuiTreeNodeFlags_DefaultOpen)) {

				ImGui::Spacing();
				ImGui::Spacing();
				ImGui::Text("Curve");
				std::vector<std::string> names = ShaderProgramPool::get().names("Transformation");
				std::vector<const char*> strings; for (int a = 0; a < names.size(); a++) {strings.push_back(names[a].c_str());}
				if (ImGui::Combo("Type", &transformationoption, strings.data(), strings.size())) {
					ShaderProgram* p = ShaderProgramPool::get().get("Transformation", names[transformationoption]);
					if (p != nullptr) {transformation = p; flag = true;}
				}

				if (transformation != nullptr) {transformation->imgui();}

				ImGui::Spacing();
				ImGui::Spacing();
				ImGui::Text("Color");
				names = ShaderProgramPool::get().names("Colormap");
				strings.clear(); for (int a = 0; a < names.size(); a++) {strings.push_back(names[a].c_str());}
				if (ImGui::Combo("Colormap", &coloroption, strings.data(), strings.size())) {
					ShaderProgram* p = ShaderProgramPool::get().get("Colormap", names[coloroption]);
					if (p != nullptr) {colormap = p; flag = true;}
				}

			}

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
	/*
	unsigned char* data = (unsigned char*) malloc(w * h * 3);
	savebuffer->bind();
	glReadPixels(0, 0, Window::getWidth(), Window::getHeight(), GL_RGB, GL_UNSIGNED_BYTE, data);
	savebuffer->unbind();
	stbi_flip_vertically_on_write(1);
	stbi_write_png("test.png", Window::getWidth(), Window::getHeight(), 3, data, Window::getWidth() * 3);
	free(data);
	*/

	// Destroy the fractalbuffers.
	delete fractalbuffer;
	delete curvebuffer;

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