#include "programs/transformation.hpp"
#include <cmath>

namespace SineWaveCode {

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
uniform float uFrequency;
uniform float uPhase;

out vec3 color;

void main() {
    float value = texture(uTexture, fTexCoords).x;
    if (value != 0.0) {value = 0.5 * sin(uFrequency * (value + uPhase)) + 0.5;}
    color = vec3(value, value, value);
})";

}

class SineWave: public TransformationProgram {

    private:

        float frequency = 1.0f;
        float phase = 0.0f;

    public:

        std::string vertex() override {
            return std::string(SineWaveCode::vertex);
        }

        std::string fragment() override {
            return std::string(SineWaveCode::fragment);
        }

        void upload() override {
            this->TransformationProgram::upload();
            Shader* s = this->getShader();
            s->uploadFloat("uFrequency", this->frequency);
            s->uploadFloat("uPhase", this->phase);
        }

        void imgui() override {
            this->TransformationProgram::imgui();
            ImGui::SliderFloat("Frequency##SineWave", &this->frequency, -16.0f, 16.0f);
            ImGui::SliderFloat("Phase##SineWave", &this->phase, -M_PI / this->frequency, M_PI / this->frequency);
        }

};

REGISTER_SHADER_PROGRAM(Transformation, SineWave);