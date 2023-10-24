#include <cstdlib>
#include <imgui.h>

#include "core/window.hpp"
#include "programs/fractal.hpp"

namespace JuliaCode {

const char* vertex = R"(#version 400 core

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

${ARBITRARY}

in vec2 fPosition;
in vec2 fTexCoords;

uniform int uIterations;
uniform uint uPositionX[ARRAY_SIZE];
uniform uint uPositionY[ARRAY_SIZE];
uniform uint uScaleX[ARRAY_SIZE];
uniform uint uScaleY[ARRAY_SIZE];
uniform uint uReal[ARRAY_SIZE];
uniform uint uImaginary[ARRAY_SIZE];

out vec3 color;

float julia(uint[ARRAY_SIZE] z_r, uint[ARRAY_SIZE] z_i) {
    
    uint tmp[ARRAY_SIZE];
    uint nz_r[ARRAY_SIZE];
    uint nz_i[ARRAY_SIZE];

    for (int k = 0; k < uIterations; k++) {

        // Compute c_r^2 + c_i^2
        uint radius[ARRAY_SIZE];
        mul(z_r, z_r, radius);
        mul(z_i, z_i, tmp);
        add(radius, tmp, radius);

        // If we are more than 2 units away, c_r^2 + c_i^2 will be greater than 4.
        if (radius[1] > 4) {
            return float(k) / float(uIterations);
        }

        // Compute the real component of the square.
        mul(z_i, z_i, nz_r);
        negate(nz_r);
        mul(z_r, z_r, tmp);
        add(nz_r, tmp, nz_r);
        add(nz_r, uReal, nz_r);

        // Compute the imaginary component of the square.
        load(nz_i, 2.0);
        mul(nz_i, z_r, nz_i);
        mul(nz_i, z_i, nz_i);
        add(nz_i, uImaginary, nz_i);

        // Update the past values.
        assign(z_r, nz_r);
        assign(z_i, nz_i);

    }

    return 0.0;
}

void main() {

    uint z_r[ARRAY_SIZE];
    load(z_r, fPosition.x);
    mul(z_r, uScaleX, z_r);
    add(z_r, uPositionX, z_r);

    uint z_i[ARRAY_SIZE];
    load(z_i, fPosition.y);
    mul(z_i, uScaleY, z_i);
    add(z_i, uPositionY, z_i);
    
    color = vec3(1.0, 1.0, 1.0) * julia(z_r, z_i);

})";

}

class Julia : public FractalProgram {

    private:

        Arbitrary real = Arbitrary(-0.7269);
        Arbitrary imaginary = Arbitrary(0.1889);

    public:

        std::string vertex() override {
            return std::string(JuliaCode::vertex);
        }

        std::string fragment() override {
            return std::string(JuliaCode::fragment);
        }

        void upload() override {

            this->FractalProgram::upload();

            Shader* s = this->getShader();
			s->uploadUnsignedIntArray("uReal", Arbitrary::precision()+1, this->real.data());
			s->uploadUnsignedIntArray("uImaginary", Arbitrary::precision()+1, this->imaginary.data());

        }

        void imgui() override {

            this->FractalProgram::imgui();

            float prev = (float) Arbitrary::value(this->real);
            float next = prev;

            ImGui::SliderFloat("Real##Julia", &next, -1.0f, 1.0f);
            if (next != prev) {this->real = Arbitrary(next); Window::update();}

            prev = (float) Arbitrary::value(this->imaginary);
            next = prev;

            ImGui::SliderFloat("Imaginary##Julia", &next, -1.0f, 1.0f);
            if (next != prev) {this->imaginary = Arbitrary(next); Window::update();}

        }

};

REGISTER_SHADER_PROGRAM(Fractal, Julia);