#version 330 core

layout (location=0) in vec2 aPosition;
layout (location=1) in vec2 aTexCoords;

uniform vec2 uPosition;
uniform vec2 uProjectionSize;

out vec2 fPosition;
out vec2 fTexCoords;

void main() {
    gl_Position = vec4(aPosition, 0.0, 1.0);
    fPosition = (aPosition * uProjectionSize * 0.5) + uPosition;
    fTexCoords = aTexCoords;
}