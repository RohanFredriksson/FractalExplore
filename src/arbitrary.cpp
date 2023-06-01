#include "arbitrary.hpp"
#include <iomanip>
#include <climits>
#include <cmath>

namespace {
    const uint32_t BASE = 4294967295;
    const int PRECISION = 3;
}

Arbitrary::Arbitrary() {
    for (int i = 0; i <= PRECISION; i++) {this->values.push_back(0);}
    this->load(0.0f);
}

Arbitrary::Arbitrary(double value) {
    for (int i = 0; i <= PRECISION; i++) {this->values.push_back(0);}
    this->load(value);
}

void Arbitrary::load(double value) {

    // Store the sign bit in the first element of the array.
    if (value < 0.0f) {this->values[0] = 1;}
    else {this->values[0] = 0;}
    value = std::abs(value);

    // Store float as a collection of uint32_t.
    for (int i = 1; i <= PRECISION; i++) {
        this->values[i] = (uint32_t) value;
        value -= this->values[i];
        value *= BASE;
    }

}

uint32_t* Arbitrary::data() {
    return this->values.data();
}

int Arbitrary::precision() {
    return PRECISION;
}

double Arbitrary::value(const Arbitrary n) {

    double result = 0.0;
    for (int i = PRECISION; i > 0; i--) {
        result /= BASE;
        result += n.values[i];
    }

    if (n.values[0] > 0) {
        result *= -1.0;
    }

    return result;

}

void Arbitrary::operator=(const Arbitrary& other) {
    for (int i = 0; i <= PRECISION; i++) {
        this->values[i] = other.values[i];
    }
}

Arbitrary Arbitrary::operator+(const Arbitrary& other) {

    Arbitrary a = *this;
    Arbitrary b = other;
    Arbitrary result;

    bool pa = a.values[0] == 0;
    bool pb = b.values[0] == 0;
    
    if (pa == pb) {
        
        // Perform addition.
        uint32_t carry = 0;
        for (int i = PRECISION; i > 0; i--) {
            result.values[i] = a.values[i] + b.values[i] + carry;
            if (result.values[i] < std::max(a.values[i], b.values[i])) {carry = 1;}
            else {carry = 0;}
        }

        // If the numbers are negative, the result is negative.
        if (!pa) {result.values[0] = 1;}

    }

    else {

        // Determine which number is larger.
        bool flip = false;
        for (int i = 1; i <= PRECISION; i++) {
            if (b.values[i] > a.values[i]) {flip = true; break;}
            if (a.values[i] > b.values[i]) {break;}
        }

        // If b is larger than a, flip the two numbers.
        if (flip) {
            uint32_t tmp;
            for (int i = 0; i <= PRECISION; i++) {
                tmp = a.values[i];
                a.values[i] = b.values[i];
                b.values[i] = tmp;
            }
        }

        // Run the basic subtraction algorithm.
        uint32_t borrow = 0;
        for (int i = PRECISION; i > 0; i--) {
            result.values[i] = a.values[i] - b.values[i] - borrow;
            if (a.values[i] < b.values[i] + borrow) {borrow = 1;} 
            else {borrow = 0;}
        }

        // Fix the sign
        if (pa == flip) {result.values[0] = 1;}
        else {result.values[0] = 0;}

    }

    return result;

}

Arbitrary Arbitrary::operator-(const Arbitrary& other) {
    Arbitrary a = *this;
    Arbitrary b = other;
    b.values[0] = b.values[0] == 0 ? 1 : 0;
    return a + b;
}

Arbitrary Arbitrary::operator*(const Arbitrary& other) {

    Arbitrary a = *this;
    Arbitrary b = other;
    Arbitrary result;

    for (int i = 0; i < PRECISION; i++) {

        for (int j = 0; j < PRECISION; j++) {

            Arbitrary partial;

            //uint32_t carry = (((double) a.values[i+1] / BASE) * (double) b.values[j+1]);
            uint32_t lower_a = a.values[i+1] & 0xFFFF;
            uint32_t upper_a = (a.values[i+1] >> 16) & 0xFFFF;
            uint32_t lower_b = b.values[j+1] & 0xFFFF;
            uint32_t upper_b = (b.values[j+1] >> 16) & 0xFFFF;
            uint32_t product_low = lower_a * lower_b;
            uint32_t product_mid = lower_a * upper_b + upper_a * lower_b;
            uint32_t product_high = upper_a * upper_b;
            product_mid += (product_low >> 16);
            product_high += (product_mid >> 16);
            uint32_t carry = product_high;

            if (i+j+1<=PRECISION) {partial.values[i+j+1] = a.values[i+1] * b.values[j+1];}
            if (i+j>=1&&i+j<=PRECISION) {partial.values[i+j] = carry;}
            result = result + partial;

        }

    }

    if ((a.values[0] == 0) != (b.values[0] == 0)) {result.values[0] = 1;}
    return result;

}