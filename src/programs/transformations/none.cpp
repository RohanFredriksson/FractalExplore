#include "programs/transformation.hpp"

namespace NoneCode {

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

void main() {
    float value = texture(uTexture, fTexCoords).x;
    color = vec3(value, value, value);
})";

}

class None : public TransformationProgram {

    public:

        std::string vertex() override {
            return std::string(NoneCode::vertex);
        }

        std::string fragment() override {
            return std::string(NoneCode::fragment);
        }

};

REGISTER_SHADER_PROGRAM(Transformation, None);