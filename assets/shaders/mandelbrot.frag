#version 330 core

in vec2 coords;

out vec4 colour;

vec2 complex_square(vec2 z) {
    return vec2(
                z.x * z.x - z.y * z.y, 
                2 * z.x * z.y
    );
}

float compute(vec2 c, int iterations) {

    vec2 z = vec2(0, 0);
    
    int k = 0;
    while (k < iterations) {
        
        z = complex_square(z) + c;
        k = k + 1;

        if (length(z) > 2) {
            break;
        }

    }

    if (k < iterations) {
        return k / iterations;
    }

    return 0.0;

}

void main() {
    colour = vec4(vec3(0.0,0.0,0.0) + compute(coords), 1.0);
}