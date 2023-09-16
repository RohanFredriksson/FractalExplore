#pragma once

#include <cstdint>
#include <vector>
#include <string>

class Arbitrary {   

    public:

        std::vector<uint32_t> values;

        Arbitrary();
        Arbitrary(double value);
        Arbitrary(std::string value);
        void load(double value);
        void load(std::string value);
        uint32_t* data();

        static int precision();
        static double value(const Arbitrary n);
        static Arbitrary shift(const Arbitrary n, int p);
        static std::string serialise(const Arbitrary n);
        static Arbitrary parse(std::string value);
        static Arbitrary negate(Arbitrary n);
        static bool validate(std::string value);
        static int max_length();

        void operator=(const Arbitrary& other);
        Arbitrary operator+(const Arbitrary& other);
        Arbitrary operator-(const Arbitrary& other);
        Arbitrary operator*(const Arbitrary& other);

        //friend std::ostream& operator<<(std::ostream& os, const Arbitrary& a);
        
};