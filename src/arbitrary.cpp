#include "arbitrary.hpp"
#include <iomanip>
#include <climits>
#include <array>
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

    /*
    std::array<uint32_t, 2*PRECISION-1> product;
    for (int i = 0; i < product.size(); i++) {product[i] = 0;}

    for (int i = 0; i < PRECISION; i++) {

        uint32_t carry = 0;

        for (int j = 0; j < PRECISION; j++) {

            uint32_t next = 0;
            uint32_t value = a.values[PRECISION-i] * b.values[PRECISION-j];
            if (product[i+j] + value < product[i+j]) {next++;}
            product[i+j] += value;
            if (product[i+j] + carry < product[i+j]) {next++;}
            product[i+j] += carry;

            uint32_t lower_a = a.values[PRECISION-i] & 0xFFFF;
            uint32_t upper_a = (a.values[PRECISION-i] >> 16) & 0xFFFF;
            uint32_t lower_b = b.values[PRECISION-j] & 0xFFFF;
            uint32_t upper_b = (b.values[PRECISION-j] >> 16) & 0xFFFF;
            uint32_t product_low = lower_a * lower_b;
            uint32_t product_mid = lower_a * upper_b + upper_a * lower_b;
            uint32_t product_high = upper_a * upper_b;
            product_mid += (product_low >> 16);
            product_high += (product_mid >> 16);
            carry = product_high + next;

        }

        if (i+PRECISION < 2*PRECISION-1) {
            product[i+PRECISION] += carry;
        }
        
    }

    // Round to the specified precision.
    if (product[product.size()-PRECISION] >= BASE/2) {
        for (int i = product[product.size()-PRECISION+1]; i < product.size(); i++) {
            if (product[i] + 1 > product[i]) {product[i]++; break;}
            product[i]++; 
        }
    }

    // Move the product into the arbitrary class.
    for (int i = 0; i < PRECISION; i++) {result.values[i+1] = product[product.size()-i-1];}
    */

    for (int i = 0; i < PRECISION; i++) {

        Arbitrary partial;
        uint32_t carry = 0;

        for (int j = 0; j < PRECISION; j++) {

            partial.values[PRECISION-j] = a.values[i+1] * b.values[PRECISION-j];
            if (partial.values[PRECISION-j] + carry < partial.values[PRECISION-j]) {partial.values[PRECISION-j] += carry; carry = 1;}
            else {partial.values[PRECISION-j] += carry; carry = 0;}

            uint32_t lower_a = a.values[i+1] & 0xFFFF;
            uint32_t upper_a = (a.values[i+1] >> 16) & 0xFFFF;
            uint32_t lower_b = b.values[PRECISION-j] & 0xFFFF;
            uint32_t upper_b = (b.values[PRECISION-j] >> 16) & 0xFFFF;
            uint32_t product_low = lower_a * lower_b;
            uint32_t product_mid = lower_a * upper_b + upper_a * lower_b;
            uint32_t product_high = upper_a * upper_b;
            product_mid += (product_low >> 16);
            product_high += (product_mid >> 16);
            carry += product_high;

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