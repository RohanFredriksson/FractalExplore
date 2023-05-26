#include "arbitrary.hpp"
#include <iomanip>
#include <cstdlib>
#include <cmath>

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
    this->load(0.0f);
}

Arbitrary::Arbitrary(float value) {
    for (int i = 0; i < PRECISION; i++) {this->values.push_back(0);}
    this->load(value);
}

void Arbitrary::load(float value) {
    
    for (int i = 0; i < PRECISION; i++) {
        this->values[i] = int(value);
        value -= int(value);
        value *= BASE;
    }

}

int* Arbitrary::data() {
    return this->values.data();
}

int Arbitrary::base() {
    return BASE;
}

int Arbitrary::precision() {
    return PRECISION;
}

int Arbitrary::sign(const Arbitrary n) {

    for (int i = 0; i < PRECISION; i++) {
        if (n.values[i] < 0) {return -1;}
        else if (n.values[i] > 0) {return 1;}
    }

    return 0;

}

float Arbitrary::value(const Arbitrary n) {

    float result = 0.0f;
    for (int i = PRECISION-1; i >= 0; i--) {
        result /= BASE;
        result += n.values[i];
    }

    return result;

}

Arbitrary Arbitrary::negate(const Arbitrary n) {

    Arbitrary result = n;
    for (int i = 0; i < PRECISION; i++) {
        result.values[i] *= -1;
    }

    return result;

}

Arbitrary Arbitrary::absolute(const Arbitrary n) {

    Arbitrary result = n;
    for (int i = 0; i < PRECISION; i++) {
        result.values[i] = abs(result.values[i]);
    }

    return result;

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

    bool s1 = a.values[0] >= 0;
    bool s2 = b.values[0] >= 0;

    int carry = 0;
    int keep = 0;

    for (int i = PRECISION-1; i > 0; i--) {
        
        round(a.values[i] + b.values[i] + carry, &carry, &keep);
        if (keep < 0) {
            keep += BASE;
            carry--;
        }
        
        result.values[i] = keep;

    }

    round(a.values[0] + b.values[0] + carry, &carry, &keep);
    result.values[0] = keep;

    if (s1 != s2 && result.values[0] < 0) {
        
        result = Arbitrary::negate(result);
        carry = 0;

        for (int i = PRECISION-1; i >= 0; i--) {

            round(result.values[i] + carry, &carry, &keep);
            if (keep < 0) {
                keep += BASE;
                carry--;
            }

            result.values[i] = keep;

        }

        result = Arbitrary::negate(result);

    }

    return result;
}

Arbitrary Arbitrary::operator-(const Arbitrary& other) {

    Arbitrary a = *this;
    Arbitrary b = Arbitrary::negate(other);
    
    return a + b;

}

Arbitrary Arbitrary::operator*(const Arbitrary& other) {

    Arbitrary a = *this;
    Arbitrary b = other;
    Arbitrary result;

    bool negate = false;

    if (a.values[0] < 0) {
        a = Arbitrary::negate(a);
        negate = !negate;
    }

    if (b.values[0] < 0) {
        b = Arbitrary::negate(b);
        negate = !negate;
    }

    int lprod = ((PRECISION - 1) * 2) + 1;
    int* prod = (int*) calloc(lprod, sizeof(int));
    for (int i = 0; i < PRECISION; i++) {
        for (int j = 0; j < PRECISION; j++) {
            prod[i+j] += a.values[PRECISION-i-1] * b.values[PRECISION-j-1];
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
        result.values[PRECISION-i-1] = prod[i + clip];
    }

    if (negate) {
        result = Arbitrary::negate(result);
    }

    free(prod);
    return result;

}

std::ostream& operator<<(std::ostream& os, const Arbitrary& a) {

    if (Arbitrary::sign(a) < 0) {os << "-";}
    Arbitrary abs = Arbitrary::absolute(a);
    os << abs.values[0] << ".";
    for (int i = 1; i < PRECISION; i++) {os << std::setw(4) << std::setfill('0') << abs.values[i];}
    return os;

}