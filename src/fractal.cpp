#include <complex>
#include "fractal.h"

double mandelbrot(double a, double b, int iterations) {
    
    std::complex<double> c(a, b);
    std::complex<double> z(0, 0);
    int k = 0;

    while (k < iterations) {

        z = z * z + c;
        k = k + 1;

        if (abs(z) > 2) {
            break;
        }

    }

    if (k < iterations) {
        return k / iterations;
    }

    return 0.0;

}