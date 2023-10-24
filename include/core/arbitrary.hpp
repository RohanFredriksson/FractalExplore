#pragma once

#include <cstdint>
#include <vector>
#include <string>

class Arbitrary {   

    public:

        std::vector<uint32_t> values;

        Arbitrary();
        Arbitrary(double value);
        Arbitrary(const std::string value);
        void load(double value);
        void load(const std::string value);
        uint32_t* data();

        static int precision();
        static std::string glsl();
        static double value(const Arbitrary n);
        static Arbitrary shift(const Arbitrary n, int p);
        static std::string serialise(const Arbitrary n);
        static Arbitrary parse(const std::string value);
        static Arbitrary negate(const Arbitrary n);
        static Arbitrary reciprocal(const Arbitrary n);
        static bool validate(const std::string value);
        static int max_length();

        void operator=(const Arbitrary& other);
        Arbitrary operator+(const Arbitrary& other);
        Arbitrary operator-(const Arbitrary& other);
        Arbitrary operator*(const Arbitrary& other);
        Arbitrary operator/(const Arbitrary& other);

        //friend std::ostream& operator<<(std::ostream& os, const Arbitrary& a);
        
};