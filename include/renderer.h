#include "shader.h"

#ifndef RENDERER_H
#define RENDERER_H

void Renderer_Init();
void Renderer_BindShader(Shader* s);
void Renderer_Render();

#endif