#pragma once

#include <vector>

using std::vector;

class Arbitrary {   

    public:

        vector<int> values;      

        Arbitrary();
        Arbitrary(float value);

        int precision();
        int base();

        int* data();
        void load(float value);
        void zero();
        void negate();

        void operator=(const Arbitrary& other);
        Arbitrary operator+(const Arbitrary& other);
        Arbitrary operator-(const Arbitrary& other);
        Arbitrary operator*(const Arbitrary& other);
        
};