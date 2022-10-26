#version 400 core
layout (location=0) in vec2 aPos;
out vec2 fPos;

void main() {
    gl_Position = vec4(aPos, 0.0, 1.0);
    fPos = aPos;
}