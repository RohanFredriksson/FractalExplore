#version 330 core

layout (location=0) in vec2 aPos;
layout (location=1) in vec2 aTexCoords;

out vec2 fPos;
out vec2 fTexCoords;

void main() {
    gl_Position = vec4(aPos.x * 0.5, aPos.y * 0.5, 0.0, 1.0);
    fPos = aPos;
    fTexCoords = aTexCoords;
}