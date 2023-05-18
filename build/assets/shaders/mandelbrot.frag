#version 330 core

in vec2 fPos;

out vec4 fColor;

vec2 complex_square(vec2 z) {
    return vec2(
                z.x * z.x - z.y * z.y, 
                2 * z.x * z.y
    );
}

float compute(vec2 c, float iterations) {

    vec2 z = vec2(0, 0);
    
    float k = 0;
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
    fColor = vec4(vec3(0.0,0.0,0.0) + compute(fPos, 64), 1.0);
}