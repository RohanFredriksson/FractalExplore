#version 330 core

layout (location=0) in vec2 aPos;
layout (location=1) in vec2 aTexCoords;

uniform mat4 uProjection;
uniform mat4 uView;

out vec2 fPos;
out vec2 fTexCoords;

void main() {
    gl_Position = uProjection * uView * vec4(aPos, 0.0, 1.0);
    fPos = aPos;
    fTexCoords = aTexCoords;
}