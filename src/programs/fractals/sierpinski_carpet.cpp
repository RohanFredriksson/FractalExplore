#include "programs/fractal.hpp"

namespace SierpinskiCarpetCode {

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

float sierpinski_carpet(uint[ARRAY_SIZE] c_r, uint[ARRAY_SIZE] c_i) {

    if (c_r[1] >= 1u) {return 0;}
    if (c_i[1] >= 1u) {return 0;}

    uint three[ARRAY_SIZE];
    uint plus_two_thirds[ARRAY_SIZE];
    uint minus_two_thirds[ARRAY_SIZE];
    uint plus_third[ARRAY_SIZE];
    uint minus_third[ARRAY_SIZE];

    load(three, 3.0);
    load(plus_two_thirds, 2.0 / 3.0);
    load(minus_two_thirds, -2.0 / 3.0);
    load(plus_third, 1.0 / 3.0);
    load(minus_third, -1.0 / 3.0);

    for (int k = 0; k < uIterations; k++) {

        // Check if the position is in the middle third on each axis.
        uint x[ARRAY_SIZE];
        uint y[ARRAY_SIZE];
        assign(x, c_r);
        assign(y, c_i);
        abs(x);
        abs(y);

        add(x, minus_third, x);
        add(y, minus_third, y);

        if (x[0] > 0u && y[0] > 0u) {return 0.0;}

        // Manipulate c_r into the new range. 
        add(c_r, plus_third, x);
        if (x[0] > 0u) {add(c_r, plus_two_thirds, c_r);}
        
        add(c_r, minus_third, x);
        if (x[0] == 0u) {add(c_r, minus_two_thirds, c_r);}

        mul(c_r, three, c_r);

        // Manipulate c_i into the new range.
        add(c_i, plus_third, y);
        if (y[0] > 0u) {add(c_i, plus_two_thirds, c_i);}

        add(c_i, minus_third, y);
        if (y[0] == 0u) {add(c_i, minus_two_thirds, c_i);}

        mul(c_i, three, c_i);

    }

    return 1;

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
    
    color = vec3(1.0, 1.0, 1.0) * sierpinski_carpet(c_r, c_i);

})";

}

class SierpinskiCarpet : public FractalProgram {

    public:

        std::string vertex() override {
            return std::string(SierpinskiCarpetCode::vertex);
        }

        std::string fragment() override {
            return std::string(SierpinskiCarpetCode::fragment);
        }

};

REGISTER_SHADER_PROGRAM(Fractal, SierpinskiCarpet);