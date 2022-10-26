#include <stdio.h>
#include "window.h"

#define MAX_VERTEX_BUFFER 512 * 1024
#define MAX_ELEMENT_BUFFER 128 * 1024

GLFWwindow* window;

struct ImGuiContext* ctx;
struct ImGuiIO* io;

FrameBuffer entityTexture;
Shader* fractalShader;

ivec2 windowSize = { 800, 800 };
float fps = -1.0f;

int Window_Init() {

    // Initialise GLFW
    if (!glfwInit()) {
		printf("ERROR::WINDOW::GLFW_INITIALIZATION_FAILED\n");
		return 1;
	}

    // Configure GLFW
    glfwDefaultWindowHints();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);
    glfwWindowHint(GLFW_FOCUS_ON_SHOW, GLFW_TRUE);

    // Create the window
    window = glfwCreateWindow(windowSize[0], windowSize[1], "Waffle Engine", NULL, NULL);
    if (window == NULL) {
        printf("ERROR::WINDOW::GLFW_WINDOW_CREATION_FAILED\n");
        return 1;
    }

    // Manage callbacks
	glfwSetKeyCallback(window, KeyListener_KeyCallback);
	glfwSetWindowSizeCallback(window, WindowListener_ResizeCallback);
	glfwSetCursorPosCallback(window, MouseListener_MousePosCallback);
	glfwSetMouseButtonCallback(window, MouseListener_MouseButtonCallback);
	glfwSetScrollCallback(window, MouseListener_MouseScrollCallback);

    // Make the OpenGl context current
	glfwMakeContextCurrent(window);

    // Enable v-sync
    glfwSwapInterval(1);

    // Make the window visible
    glfwShowWindow(window);

    // Load GLAD so it configures OpenGL
	gladLoadGL();

    // Initialise cimgui
    ctx = igCreateContext(NULL);
    io  = igGetIO();

    const char* glsl_version = "#version 330 core";
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init(glsl_version);

    // Setup style
    igStyleColorsDark(NULL);

    // Set the window configuration.
    //Window_SetFullscreenWindowed();

    // Set up the window attributes.
    Camera_Init();
    Renderer_Init();
    FrameBuffer_Init(&entityTexture, windowSize[0], windowSize[1]);
    fractalShader = ShaderPool_Get("./assets/shaders/mandelbrot.vert", "./assets/shaders/mandelbrot.frag");
    Renderer_BindShader(fractalShader);
    return 0;
}

void Window_Loop() {

    float beginTime = (float)glfwGetTime();
    float endTime = (float)glfwGetTime();
    float dt = -1.0f;

    while (!glfwWindowShouldClose(window)) {

        glfwPollEvents();
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        if (dt > 0) {

            //----
            ImGui_ImplOpenGL3_NewFrame();
            ImGui_ImplGlfw_NewFrame();
            igNewFrame();
            igShowDemoWindow(NULL);
            //----

            Renderer_Render();

            // Render the imgui
            igRender();
            ImGui_ImplOpenGL3_RenderDrawData(igGetDrawData());

        }

        glfwSwapBuffers(window);
        endTime = (float)glfwGetTime();
        dt = endTime - beginTime;
        beginTime = endTime;
        fps = 1/dt;

    }

}

void Window_Exit() {

    // Free the framebuffers
    FrameBuffer_Free(&entityTexture);

    // Terminate imgui
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    igDestroyContext(ctx);

    // Delete window before ending the program
	glfwDestroyWindow(window);

	// Terminate GLFW before ending the program
	glfwTerminate();

}

void Window_Run() {

    ShaderPool_Init();
    if (Window_Init()) {return;}
    Window_Loop();
    Window_Exit();
    ShaderPool_Free();

}

int Window_GetWidth() {
    return windowSize[0];
}

int Window_GetHeight() {
    return windowSize[1];
}

void Window_SetWidth(int width) {
    windowSize[0] = width;
}

void Window_SetHeight(int height) {
    windowSize[1] = height;
}

double Window_GetAspectRatio() {
    return (double) windowSize[0] / (double) windowSize[1];
}

void Window_SetWindowed() {

    const GLFWvidmode* mode = glfwGetVideoMode(Window_GetCurrentMonitor());
    int windowWidth = (int)((float)mode->width / 2.25f);
    int windowHeight = (int)((float)mode->height / 2.25f);
    
    // Set window attributes
    glfwSetWindowAttrib(window, GLFW_RESIZABLE, GLFW_TRUE);
    glfwSetWindowAttrib(window, GLFW_DECORATED, GLFW_TRUE);
    glfwRestoreWindow(window);
    
    // Set the window size
    glfwSetWindowSize(window, windowWidth, windowHeight);

    // Set the window position.
    int x, y;
    glfwGetMonitorPos(Window_GetCurrentMonitor(), &x, &y);
    glfwSetWindowPos(window, x + (mode->width - windowWidth) / 2, y + (mode->height - windowHeight) / 2);

}

void Window_SetFullscreen() {

    // Set the window attributes.
    glfwSetWindowAttrib(window, GLFW_RESIZABLE, GLFW_FALSE);
    glfwSetWindowAttrib(window, GLFW_DECORATED, GLFW_FALSE);
    glfwMaximizeWindow(window);

    // Set the window position.
    int x, y;
    glfwGetMonitorPos(Window_GetCurrentMonitor(window), &x, &y);
    glfwSetWindowPos(window, x, y);

    // Set the window size.
    const GLFWvidmode* mode = glfwGetVideoMode(Window_GetCurrentMonitor());
    glfwSetWindowSize(window, mode->width, mode->height);

}

void Window_SetFullscreenWindowed() {

    // Set the window attributes.
    glfwSetWindowAttrib(window, GLFW_RESIZABLE, GLFW_FALSE);
    glfwSetWindowAttrib(window, GLFW_DECORATED, GLFW_FALSE);
    glfwSetWindowAttrib(window, GLFW_MAXIMIZED, GLFW_FALSE);
    glfwRestoreWindow(window);

    // Set the window position.
    int x, y;
    glfwGetMonitorPos(Window_GetCurrentMonitor(window), &x, &y);
    glfwSetWindowPos(window, x, y);

    // Set the window size.
    const GLFWvidmode* mode = glfwGetVideoMode(Window_GetCurrentMonitor());
    glfwSetWindowSize(window, mode->width, mode->height);

}

GLFWmonitor* Window_GetCurrentMonitor() {

    int wx, wy, ww, wh;
    int mx, my, mw, mh;

    int overlap, bestOverlap;
    GLFWmonitor* bestMonitor;
    GLFWmonitor** monitors;
    int numMonitors;

    bestOverlap = 0;
    bestMonitor = glfwGetPrimaryMonitor();

    glfwGetWindowPos(window, &wx, &wy);
    glfwGetWindowSize(window, &ww, &wh);
    monitors = glfwGetMonitors(&numMonitors);

    for (int i = 0; i < numMonitors; i++) {
        
        GLFWmonitor* monitor = monitors[i];
        const GLFWvidmode* mode = glfwGetVideoMode(monitor);
        glfwGetMonitorPos(monitor, &mx, &my);
        mw = mode->width;
        mh = mode->height;

        overlap = maxi(0, mini(wx + ww, mx + mw) - maxi(wx, mx)) * 
                  maxi(0, mini(wy + wh, my + mh) - maxi(wy, my));

        if (bestOverlap < overlap) {
            bestOverlap = overlap;
            bestMonitor = monitor;
        }
    }

    return bestMonitor;
}

void Window_ResetFramebuffers() {
    FrameBuffer_Free(&entityTexture);
    FrameBuffer_Init(&entityTexture, windowSize[0], windowSize[1]);
}