#include "arbitrary.hpp"
#include <cstdlib>

namespace {

    const int BASE = 10000;
    const int PRECISION = 5;

    void round(int x, int* carry, int* keep) {
        *carry = x / BASE;
        *keep = x - *carry * BASE;
    }

}

Arbitrary::Arbitrary() {
    for (int i = 0; i < PRECISION; i++) {this->values.push_back(0);}
    this->zero();
}

Arbitrary::Arbitrary(float value) {
    for (int i = 0; i < PRECISION; i++) {this->values.push_back(0);}
    this->load(value);
}

int Arbitrary::precision() {
    return PRECISION;
}

int Arbitrary::base() {
    return BASE;
}

int* Arbitrary::data() {
    return this->values.data();
}

void Arbitrary::load(float value) {
    
    for (int i = PRECISION-1; i >= 0; i--) {
        this->values[i] = int(value);
        value -= int(value);
        value *= BASE;
    }

}

void Arbitrary::zero() {

    for (int i = 0; i < PRECISION; i++) {
        this->values[i] = 0;
    }

}

void Arbitrary::negate() {

    for (int i = 0; i < PRECISION; i++) {
        this->values[i] *= -1;
    }

}

void Arbitrary::operator=(const Arbitrary& other) {

    for (int i = 0; i < PRECISION; i++) {
        this->values[i] = other.values[i];
    }

}

Arbitrary Arbitrary::operator+(const Arbitrary& other) {

    Arbitrary a = *this;
    Arbitrary b = other;
    Arbitrary result;

    bool s1 = a.values[PRECISION-1] >= 0;
    bool s2 = b.values[PRECISION-1] >= 0;

    int carry = 0;
    int keep = 0;

    for (int i = 0; i < PRECISION-1; i++) {
        
        round(a.values[i] + b.values[i] + carry, &carry, &keep);
        if (keep < 0) {
            keep += BASE;
            carry--;
        }
        
        result.values[i] = keep;

    }

    round(a.values[PRECISION-1] + b.values[PRECISION-1] + carry, &carry, &keep);
    result.values[PRECISION-1] = keep;

    if (s1 != s2 && result.values[PRECISION-1] < 0) {
        
        result.negate();
        carry = 0;

        for (int i = 0; i < PRECISION; i++) {

            round(result.values[i] + carry, &carry, &keep);
            if (keep < 0) {
                keep += BASE;
                carry--;
            }

            result.values[i] = keep;

        }

        result.negate();

    }

    return result;
}

Arbitrary Arbitrary::operator-(const Arbitrary& other) {

    Arbitrary a = *this;
    Arbitrary b = other;
    b.negate();
    
    return a + b;

}

Arbitrary Arbitrary::operator*(const Arbitrary& other) {

    Arbitrary a = *this;
    Arbitrary b = other;
    Arbitrary result;

    bool negate = false;

    if (a.values[PRECISION-1] < 0) {
        a.negate();
        negate = !negate;
    }

    if (b.values[PRECISION-1] < 0) {
        b.negate();
        negate = !negate;
    }

    int lprod = ((PRECISION - 1) * 2) + 1;
    int* prod = (int*) calloc(lprod, sizeof(int));
    for (int i = 0; i < PRECISION; i++) {
        for (int j = 0; j < PRECISION; j++) {
            prod[i+j] += a.values[i] * b.values[j];
        }
    }

    int carry = 0;
    int keep = 0;

    int clip = lprod - PRECISION;
    for (int i = 0; i < clip; i++) {
        round(prod[i] + carry, &carry, &keep);
        prod[i] = keep;
    }

    if(prod[clip-1] >= BASE/2) {
        carry++;
    }

    for (int i = clip; i < lprod; i++) {
        round(prod[i] + carry, &carry, &keep);
        prod[i] = keep;
    }

    for (int i = 0; i < lprod - clip; i++) {
        result.values[i] = prod[i + clip];
    }

    if (negate) {
        result.negate();
    }

    free(prod);
    return result;

}