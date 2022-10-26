#include "external.h"

#include "camera.h"
#include "framebuffer.h"
#include "listener.h"
#include "renderer.h"
#include "shader.h"
#include "shaderpool.h"
#include "texture.h"
#include "util.h"

#ifndef WINDOW_H
#define WINDOW_H

void Window_Run();
int Window_Init();
void Window_Loop();
void Window_Exit();

int Window_GetWidth();
int Window_GetHeight();
void Window_SetWidth(int width);
void Window_SetHeight(int height);
double Window_GetAspectRatio();
Camera* Window_GetCamera();

void Window_SetWindowed();
void Window_SetFullscreen();
void Window_SetFullscreenWindowed();
GLFWmonitor* Window_GetCurrentMonitor();

void Window_ResetFramebuffers();

#endif