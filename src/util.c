#include "util.h"

int maxi(int a, int b) {
    return (a > b) ? a : b;
}

int mini(int a, int b) {
    return (a > b) ? b : a;
}

void split_double(double input, float* high, float* low) {
    *high = (float) input;
    double delta = input - ((double) *high);
    *low = (float) delta;
}