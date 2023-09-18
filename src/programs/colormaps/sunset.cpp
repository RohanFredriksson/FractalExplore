#include "programs/colormap.hpp"

namespace SunsetCode {

const char* vertex = R"(
#version 400 core

layout (location=0) in vec2 aPosition;
layout (location=1) in vec2 aTexCoords;

out vec2 fPosition;
out vec2 fTexCoords;

void main() {
    gl_Position = vec4(aPosition, 0.0, 1.0);
    fPosition = aPosition;
    fTexCoords = aTexCoords;
})";

const char* fragment = R"(#version 400 core

in vec2 fPosition;
in vec2 fTexCoords;

uniform sampler2D uTexture;

out vec3 color;

vec3 interpolate(vec3 from, vec3 to, float start, float end, float value) {
    float t = clamp((value - start) / (end - start), 0.0, 1.0);
    return mix(from, to, t);
}

vec3 sunset(float v) {
    if (v <= 0.0) {return vec3(0.0, 0.0, 0.0);}
    if (v < 0.45) {return interpolate(vec3(252.0/255.0, 176.0/255.0, 69.0/255.0), vec3(252.0/255.0, 100.0/255.0, 100.0/255.0), 0.0, 0.45, v);}
    return interpolate(vec3(252.0/255.0, 100.0/255.0, 100.0/255.0), vec3(131.0/255.0, 58.0/255.0, 180.0/255.0), 0.45, 1.0, v);
}

void main() {
    float value = texture(uTexture, fTexCoords).x;
    color = sunset(value);
})";

}

class Sunset : public ColormapProgram {

    public:

        std::string vertex() override {
            return std::string(SunsetCode::vertex);
        }

        std::string fragment() override {
            return std::string(SunsetCode::fragment);
        }

};

REGISTER_SHADER_PROGRAM(Colormap, Sunset);