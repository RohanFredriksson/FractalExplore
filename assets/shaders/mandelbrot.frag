#version 400 core

in vec2 fPos;

uniform float xMinHigh;
uniform float xMinLow;
uniform float yMinHigh;
uniform float yMinLow;
uniform float xMaxHigh;
uniform float xMaxLow;
uniform float yMaxHigh;
uniform float yMaxLow;

out vec4 fColor;

dvec2 complex_square(dvec2 z) {
    return dvec2(
                z.x * z.x - z.y * z.y, 
                2 * z.x * z.y
    );
}

float compute(dvec2 c, float iterations) {

    dvec2 z = dvec2(0, 0);
    
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

    double xMin = double(xMinLow) + double(xMinHigh);
    double yMin = double(yMinLow) + double(yMinHigh);
    double xMax = double(xMaxLow) + double(xMaxHigh);
    double yMax = double(yMaxLow) + double(yMaxHigh);

    double width = xMax - xMin;
    double height = yMax - yMin;
    double x = xMin + width * ((fPos.x + 1) / 2);
    double y = yMin + height * ((fPos.y + 1) / 2);

    dvec2 pos = dvec2(x, y);
    fColor = vec4(vec3(0.0,0.0,0.0) + compute(pos, 64), 1.0);

}