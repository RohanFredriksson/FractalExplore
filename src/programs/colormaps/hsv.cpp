#include "programs/colormap.hpp"

namespace HSVCode {

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

vec3 hsv2rgb(vec3 c) {
    vec4 K = vec4(1.0, 2.0 / 3.0, 1.0 / 3.0, 3.0);
    vec3 p = abs(fract(c.xxx + K.xyz) * 6.0 - K.www);
    return c.z * mix(K.xxx, clamp(p - K.xxx, 0.0, 1.0), c.y);
}

void main() {

    float value = texture(uTexture, fTexCoords).x;
    if (value == 0.0) {
        color = vec3(0.0, 0.0, 0.0);
    } 
    
    else {
        vec3 rgb = hsv2rgb(vec3(value, 1.0, 1.0));
        color = vec3(rgb.x, rgb.y, rgb.z);
    }
    
})";

}

class HSV : public ColormapProgram {

    public:

        std::string vertex() override {
            return std::string(HSVCode::vertex);
        }

        std::string fragment() override {
            return std::string(HSVCode::fragment);
        }

};

REGISTER_SHADER_PROGRAM(Colormap, HSV);