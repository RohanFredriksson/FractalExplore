#pragma once

#include <cstdint>
#include <vector>

class Arbitrary {   

    public:

        std::vector<uint32_t> values;

        Arbitrary();
        Arbitrary(double value);
        void load(double value);
        uint32_t* data();

        static int precision();
        static double value(const Arbitrary n);
        static Arbitrary shift(const Arbitrary n, int p);

        void operator=(const Arbitrary& other);
        Arbitrary operator+(const Arbitrary& other);
        Arbitrary operator-(const Arbitrary& other);
        Arbitrary operator*(const Arbitrary& other);

        //friend std::ostream& operator<<(std::ostream& os, const Arbitrary& a);
        
};