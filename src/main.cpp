#include <iostream>
#include <iomanip>
#include <cstdlib>
#include <string>
#include <chrono>
#include <cctype>
#include <ctime>
#include <regex>

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb/stb_image_write.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb/stb_image.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/ext.hpp>

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

#include "assets/icon.hpp"
#include "core/arbitrary.hpp"
#include "core/window.hpp"
#include "core/listener.hpp"
#include "core/camera.hpp"
#include "graphics.hpp"

#include "programs/program.hpp"

namespace {

	struct FractalWindow {
		bool opened;
		int fractalOption;
	};

	struct CameraWindow {
		bool opened;
	};

	struct PostprocessingWindow {
		bool opened;
		int transformationOption;
		int colorOption;
	};

	struct SaveWindow {
		bool opened;
		bool entered;
		char filename[256];
	};

	struct PopupWindow {
		float time;
		std::string title;
		std::string message;
	};

	GLFWwindow* window;
	Framebuffer* fractalbuffer = nullptr;
	Framebuffer* curvebuffer = nullptr;
	Framebuffer* colorbuffer = nullptr;

	ShaderProgram* fractal = nullptr;
	ShaderProgram* transformation = nullptr;
	ShaderProgram* colormap = nullptr;

	FractalWindow fractalWindow = { false, 0 };
	CameraWindow cameraWindow = { false };
	PostprocessingWindow postprocessingWindow = { false, 0, 0 };
	PopupWindow popupWindow = { 0.0f, "", "" };
	SaveWindow saveWindow = { false, false, "screenshot.png" };

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

std::string timestamp() {
	auto now = std::chrono::system_clock::now();
	std::time_t current = std::chrono::system_clock::to_time_t(now);
	std::tm tm = *std::localtime(&current);
	std::stringstream ss;
	ss << std::put_time(&tm, "%Y-%m-%d_%H.%M.%S");
	return ss.str();
}

std::string trim(const std::string string) {
	std::string result(string);
	size_t first = result.find_first_not_of(" \t\n\r\f\v");
	if (first == std::string::npos) {return "";}
	size_t last = result.find_last_not_of(" \t\n\r\f\v");
	return result.substr(first, last - first + 1);
}

bool validate(const std::string filename) {
	std::regex pattern("^[\\w\\-. ]+$");
	return std::regex_match(filename, pattern);
}

void saveImage() {

	std::string filename(saveWindow.filename);
	filename = trim(filename);
	if (filename == "") {filename = timestamp() + ".png";}

	if (validate(filename)) {

		int width = Window::getWidth() - ((fractalWindow.opened || cameraWindow.opened || postprocessingWindow.opened) ? 300 : 0);
		int height = Window::getHeight();

		unsigned char* data = (unsigned char*) malloc(width * height * 3);
		colorbuffer->bind();
		glReadPixels(0, 0, width, height, GL_RGB, GL_UNSIGNED_BYTE, data);
		colorbuffer->unbind();
		stbi_flip_vertically_on_write(1);
		stbi_write_png(filename.c_str(), width, height, 3, data, width * 3);
		free(data);

		popupWindow.time = 4.0f;
		popupWindow.title = "Image saved.";
		popupWindow.message = "Image successfully saved to \"" + filename + "\".";

		saveWindow.opened = false;

	}

	else {

		popupWindow.time = 4.0f;
		popupWindow.title = "Image could not be saved.";
		popupWindow.message = "\"" + filename + "\" is not a valid filename.";

	}

} 

void resize(GLFWwindow* window, int width, int height) {

	w = width;
	h = height;

	if (fractalbuffer != nullptr) {delete fractalbuffer;} fractalbuffer = new Framebuffer(GL_RGB, width, height, GL_RGB, GL_UNSIGNED_BYTE);
	if (curvebuffer != nullptr) {delete curvebuffer;} curvebuffer = new Framebuffer(GL_RGB, width, height, GL_RGB, GL_UNSIGNED_BYTE);
	if (colorbuffer != nullptr) {delete colorbuffer;} colorbuffer = new Framebuffer(GL_RGB, width, height, GL_RGB, GL_UNSIGNED_BYTE);

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

	// Load the icon
	int width, height, channels;
	unsigned char* image = stbi_load_from_memory(Icon::bytes, Icon::length, &width, &height, &channels, 0);
	if (image != nullptr) {
		GLFWimage icon;
		icon.width = width;
		icon.height = height;
		icon.pixels = image;
		glfwSetWindowIcon(window, 1, &icon);
		stbi_image_free(image);
	}

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
	ImGuiIO& io = ImGui::GetIO(); (void)io; io.IniFilename = NULL;
	ImGui::StyleColorsDark();
	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init("#version 330");

	// Fractal Shader Program
	fractal = ShaderProgramPool::get().get("Fractal", "Mandelbrot");
	std::vector<std::string> names = ShaderProgramPool::get().names("Fractal");
	for (int a = 0; a < names.size(); a++) {if (names[a] == "Mandelbrot") {fractalWindow.fractalOption = a; break;}}
	
	// Transformation Shader Program
	transformation = ShaderProgramPool::get().get("Transformation", "None");
	names = ShaderProgramPool::get().names("Transformation");
	for (int a = 0; a < names.size(); a++) {if (names[a] == "None") {postprocessingWindow.transformationOption = a; break;}}

	// Colormap Shader Program
	colormap = ShaderProgramPool::get().get("Colormap", "HSV");
	names = ShaderProgramPool::get().names("Colormap");
	for (int a = 0; a < names.size(); a++) {if (names[a] == "HSV") {postprocessingWindow.colorOption = a; break;}}

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

		if (KeyListener::isKeyBeginDown(GLFW_KEY_S) && (KeyListener::isKeyDown(GLFW_KEY_LEFT_CONTROL) || KeyListener::isKeyDown(GLFW_KEY_RIGHT_CONTROL))) {
			saveWindow.opened = true;
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
		colorbuffer->bind();
		renderer.render();
		colorbuffer->unbind();
		curvebuffer->getTexture()->unbind();

		// Imgui
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		// Main menu bar
		bool before = fractalWindow.opened || cameraWindow.opened || postprocessingWindow.opened;
        if (ImGui::BeginMainMenuBar()) {
            if (ImGui::BeginMenu("File")) {
                if (ImGui::MenuItem("Save", "Ctrl+S")) {saveWindow.opened = !saveWindow.opened;}
                if (ImGui::MenuItem("Quit", "Alt+F4")) {glfwSetWindowShouldClose(window, true);}
                ImGui::EndMenu();
            }

            if (ImGui::BeginMenu("Configure")) {
                if (ImGui::MenuItem("Fractal", (fractalWindow.opened ? "+": "-"))) {fractalWindow.opened = !fractalWindow.opened;}
				if (ImGui::MenuItem("Camera", (cameraWindow.opened ? "+": "-"))) {cameraWindow.opened = !cameraWindow.opened;}
				if (ImGui::MenuItem("PostProcessing", (postprocessingWindow.opened ? "+": "-"))) {postprocessingWindow.opened = !postprocessingWindow.opened;}
                ImGui::EndMenu();
            }

            ImGui::EndMainMenuBar();
        }
		bool after = fractalWindow.opened || cameraWindow.opened || postprocessingWindow.opened;
		
		// Check if the window is maximised.
		int maximized = glfwGetWindowAttrib(window, GLFW_MAXIMIZED);

		// If the side window opened.
		if (!before && after && maximized == 0) {
			Arbitrary dx = Arbitrary(150) * ((Camera::getWidth() * Camera::getDepth()) / Window::getWidth());
			Camera::setX(Camera::getX() + dx);
			glfwSetWindowSize(window, Window::getWidth() + 300, Window::getHeight());
			Window::update();
		}

		// If the side window closed.
		else if (before && !after && maximized == 0) {
			Arbitrary dx = Arbitrary(150) * ((Camera::getWidth() * Camera::getDepth()) / Window::getWidth());
			Camera::setX(Camera::getX() - dx);
			glfwSetWindowSize(window, Window::getWidth() - 300, Window::getHeight());
			Window::update();
		}

		if (fractalWindow.opened || cameraWindow.opened || postprocessingWindow.opened) {

			ImGui::SetNextWindowSize(ImVec2(300, Window::getHeight()));
			ImGui::SetNextWindowPos(ImVec2(ImGui::GetIO().DisplaySize.x - 300, 18), ImGuiCond_Always);
			ImGui::Begin("#Configure", nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoTitleBar);
			
			// Fractal Properties
			ImGui::Spacing();
            if (fractalWindow.opened && ImGui::CollapsingHeader("Fractal", ImGuiTreeNodeFlags_DefaultOpen)) {

				ImGui::Spacing();
				ImGui::Spacing();
				ImGui::Text("Fractal");
				std::vector<std::string> names = ShaderProgramPool::get().names("Fractal");
				std::vector<const char*> strings; for (int a = 0; a < names.size(); a++) {strings.push_back(names[a].c_str());}
				if (ImGui::Combo("Type##Fractal", &fractalWindow.fractalOption, strings.data(), strings.size())) {
					ShaderProgram* p = ShaderProgramPool::get().get("Fractal", names[fractalWindow.fractalOption]);
					if (p != nullptr) {fractal = p; flag = true;}
				}
				if (fractal != nullptr) {fractal->imgui();}

				ImGui::Spacing();
				ImGui::Spacing();

            }
			
			// Camera Properties
			ImGui::Spacing();
			if (cameraWindow.opened && ImGui::CollapsingHeader("Camera", ImGuiTreeNodeFlags_DefaultOpen)) {

				ImGui::Spacing();
				ImGui::Spacing();
				ImGui::Text("Position");

				int length = Arbitrary::max_length();
				char* buffer = (char*) malloc(length+1);

				std::string next = Arbitrary::serialise(Camera::getX());
				memcpy(buffer, next.c_str(), next.length()+1);
				ImGui::InputText("X##Camera", buffer, length);
				if (strcmp(buffer, next.c_str()) != 0) {
					
					std::string candidate(buffer);
					if (Arbitrary::validate(candidate)) {
						Camera::setX(Arbitrary(candidate));
						flag = true;
					}

				}

				next = Arbitrary::serialise(Camera::getY()); 
				memcpy(buffer, next.c_str(), next.length()+1);
				ImGui::InputText("Y##Camera", buffer, length);
				if (strcmp(buffer, next.c_str()) != 0) {
					
					std::string candidate(buffer);
					if (Arbitrary::validate(candidate)) {
						Camera::setY(Arbitrary(candidate));
						flag = true;
					}

				}

				ImGui::Spacing();
				ImGui::Spacing();
				ImGui::Text("Zoom");

				next = Arbitrary::serialise(Arbitrary::reciprocal(Camera::getDepth()));
				memcpy(buffer, next.c_str(), next.length()+1);
				ImGui::InputText("Zoom##Camera", buffer, length);
				if (strcmp(buffer, next.c_str()) != 0) {
					
					std::string candidate(buffer);
					if (Arbitrary::validate(candidate)) {
						Camera::setDepth(Arbitrary::reciprocal(Arbitrary(candidate)));
						flag = true;
					}

				}

				free(buffer);

				ImGui::Spacing();
				ImGui::Spacing();

			}

			// Postprocessing Properties
			ImGui::Spacing();
			if (postprocessingWindow.opened && ImGui::CollapsingHeader("PostProcessing", ImGuiTreeNodeFlags_DefaultOpen)) {

				ImGui::Spacing();
				ImGui::Spacing();
				ImGui::Text("Curve");
				std::vector<std::string> names = ShaderProgramPool::get().names("Transformation");
				std::vector<const char*> strings; for (int a = 0; a < names.size(); a++) {strings.push_back(names[a].c_str());}
				if (ImGui::Combo("Type##PostProcessing", &postprocessingWindow.transformationOption, strings.data(), strings.size())) {
					ShaderProgram* p = ShaderProgramPool::get().get("Transformation", names[postprocessingWindow.transformationOption]);
					if (p != nullptr) {transformation = p; flag = true;}
				}

				if (transformation != nullptr) {transformation->imgui();}

				ImGui::Spacing();
				ImGui::Spacing();
				ImGui::Text("Color");
				names = ShaderProgramPool::get().names("Colormap");
				strings.clear(); for (int a = 0; a < names.size(); a++) {strings.push_back(names[a].c_str());}
				if (ImGui::Combo("Colormap##PostProcessing", &postprocessingWindow.colorOption, strings.data(), strings.size())) {
					ShaderProgram* p = ShaderProgramPool::get().get("Colormap", names[postprocessingWindow.colorOption]);
					if (p != nullptr) {colormap = p; flag = true;}
				}

				ImGui::Spacing();
				ImGui::Spacing();

			}

			ImGui::End();
		}

		if (saveWindow.opened) {

			// Window configuration
			ImGui::SetNextWindowSize(ImVec2(400, 77));
			ImGui::SetNextWindowPos(ImVec2(10, 28), ImGuiCond_Always);
			ImGui::Begin("Save##Save", nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoMove);
            
			// Filename input field
			ImGui::Text("Filename:");
			ImGui::SameLine();
			ImGui::SetNextItemWidth(ImGui::GetWindowWidth() - ImGui::CalcTextSize("Filename:").x - 23.0f);
            if (ImGui::InputText("##SaveFilename", saveWindow.filename, sizeof(saveWindow.filename), ImGuiInputTextFlags_EnterReturnsTrue, nullptr)) {
				saveImage();
			}

			// Save Button
			ImGui::SetCursorPosX(ImGui::GetWindowWidth() - ImGui::CalcTextSize("Save").x - ImGui::CalcTextSize("Cancel").x - 30.0f);
            if (ImGui::Button("Save##SaveSave")) {
				saveImage();
            }

			// Cancel Button
			ImGui::SameLine();
            ImGui::SetCursorPosX(ImGui::GetWindowWidth() - ImGui::CalcTextSize("Cancel").x - 15.0f); // Adjust 20.0f as needed
            if (ImGui::Button("Cancel##SaveCancel")) {
                saveWindow.opened = false;
            }

			ImGui::End();

		}

		if (popupWindow.time > 0.0f) {

			// Window configuration
			ImGui::SetNextWindowSize(ImVec2(340, 50));
			ImGui::SetNextWindowPos(ImVec2(ImGui::GetIO().DisplaySize.x - 350 - ((fractalWindow.opened || cameraWindow.opened || postprocessingWindow.opened) ? 300 : 0), 28), ImGuiCond_Always);
			ImGui::Begin(popupWindow.title.c_str(), nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoMove);
			ImGui::Text(popupWindow.message.c_str());
			ImGui::End();

			popupWindow.time -= dt;

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

	// Destroy the fractalbuffers.
	delete fractalbuffer;
	delete curvebuffer;
	delete colorbuffer;

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