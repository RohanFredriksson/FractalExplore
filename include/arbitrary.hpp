#pragma once

#include <vector>
#include <string>
#include <iostream>

using std::vector;
using std::string;

class Arbitrary {   

    public:

        vector<int> values;      

        Arbitrary();
        Arbitrary(float value);
        void load(float value);
        int* data();

        static int base();
        static int precision();

        static int sign(const Arbitrary n);
        static float value(const Arbitrary n);
        static Arbitrary negate(const Arbitrary n);
        static Arbitrary absolute(const Arbitrary n);

        void operator=(const Arbitrary& other);
        Arbitrary operator+(const Arbitrary& other);
        Arbitrary operator-(const Arbitrary& other);
        Arbitrary operator*(const Arbitrary& other);

        friend std::ostream& operator<<(std::ostream& os, const Arbitrary& a);

        //static Arbitrary reciprocal(const Arbitrary n);
        //Arbitrary operator/(const Arbitrary& other);
        //bool operator>(const Arbitrary& other);
        
};