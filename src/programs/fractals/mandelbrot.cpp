#include "programs/fractal.hpp"

namespace MandelbrotCode {

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

out vec3 color;

float mandelbrot(uint[ARRAY_SIZE] c_r, uint[ARRAY_SIZE] c_i) {

    uint z_r[ARRAY_SIZE];
    uint z_i[ARRAY_SIZE];
    zero(z_r);
    zero(z_i);
    
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
        add(nz_r, c_r, nz_r);

        // Compute the imaginary component of the square.
        load(nz_i, 2.0);
        mul(nz_i, z_r, nz_i);
        mul(nz_i, z_i, nz_i);
        add(nz_i, c_i, nz_i);

        // Update the past values.
        assign(z_r, nz_r);
        assign(z_i, nz_i);

    }

    return 0.0;
}

void main() {

    uint c_r[ARRAY_SIZE];
    load(c_r, fPosition.x);
    mul(c_r, uScaleX, c_r);
    add(c_r, uPositionX, c_r);

    uint c_i[ARRAY_SIZE];
    load(c_i, fPosition.y);
    mul(c_i, uScaleY, c_i);
    add(c_i, uPositionY, c_i);
    
    color = vec3(1.0, 1.0, 1.0) * mandelbrot(c_r, c_i);

})";

}

class Mandelbrot : public FractalProgram {

    public:

        std::string vertex() override {
            return std::string(MandelbrotCode::vertex);
        }

        std::string fragment() override {
            return std::string(MandelbrotCode::fragment);
        }

};

REGISTER_SHADER_PROGRAM(Fractal, Mandelbrot);