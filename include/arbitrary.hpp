#pragma once

#include <vector>

using std::vector;

class Arbitrary {   

    public:

        int* values;      

        Arbitrary();
        Arbitrary(float value);
        ~Arbitrary();

        int precision();
        int base();

        void load(float value);
        void zero();
        void negate();

        void operator=(const Arbitrary& other);
        Arbitrary operator+(const Arbitrary& other);
        Arbitrary operator*(const Arbitrary& other);

};