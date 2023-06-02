#include "arbitrary.hpp"
#include <iomanip>
#include <climits>
#include <array>
#include <cmath>

namespace {
    const double BASE = 4294967296.0;
    const int PRECISION = 2;
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

Arbitrary Arbitrary::shift(const Arbitrary n, int p) {
    Arbitrary result = n;
    for (int i = p+1; i <= PRECISION; i++) {result.values[i] = result.values[i-p];}
    for (int i = 1; i <= p; i++) {result.values[i] = 0;}
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
            uint32_t next = 0;
            if (a.values[i] + b.values[i] < a.values[i]) {next=1;}
            result.values[i] = a.values[i] + b.values[i] + carry;
            carry = next;
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

        Arbitrary partial;
        uint32_t carry = 0;

        for (int j = 0; j < PRECISION; j++) {
            uint32_t next = 0;
            partial.values[PRECISION-j] = a.values[i+1] * b.values[PRECISION-j];
            if (partial.values[PRECISION-j] + carry < partial.values[PRECISION-j]) {next++;}
            partial.values[PRECISION-j] += carry;
	        next += (uint32_t) (((double) a.values[i+1] * (double) b.values[PRECISION-j]) / BASE); // TODO FIND A BETTER WAY TO DO THIS.
            carry = next;
        }

        if (i > 0) {
            partial = Arbitrary::shift(partial, i);
            partial.values[i] = carry;
        }

        result = result + partial;

    }

    // Fix the sign.
    if ((a.values[0] == 0) != (b.values[0] == 0)) {result.values[0] = 1;}
    return result;

}