#include "core/arbitrary.hpp"
#include <stdexcept>
#include <sstream>
#include <iomanip>
#include <limits>
#include <regex>
#include <cmath>
#include <array>

namespace {

    const double BASE = 4294967296.0;
    const int PRECISION = 8;

    const std::string GLSL = std::string(R"(
const int PRECISION = ${PRECISION};
const int ARRAY_SIZE = (PRECISION+1);
const float BASE = 4294967296.0;
const uint HALF_BASE = 2147483648u;

#define assign(x, y) {for(int assign_i=0;assign_i<=PRECISION;assign_i++){x[assign_i]=y[assign_i];}}
#define zero(x) {for(int zero_i=0;zero_i<=PRECISION;zero_i++){x[zero_i]=0u;}}
#define load(x, v) {float load_value=(v); if (load_value<0.0) {x[0]=1u; load_value*=-1.0;} else {x[0]=0u;} for(int load_i=1; load_i<=PRECISION; load_i++) {x[load_i]=uint(load_value); load_value-=x[load_i]; load_value*=BASE;}}
#define shift(x, v) {int shift_n=(v); for(int shift_i=shift_n+1; shift_i<=PRECISION; shift_i++) {x[shift_i]=x[shift_i-shift_n];} for(int shift_i=1; shift_i<=shift_n; shift_i++) {x[shift_i]=0u;}};
#define negate(x) {x[0]=(x[0]==0u?1u:0u);}
#define abs(x) {x[0]=0u;}
#define add(a, b, r) {uint add_buffer[PRECISION+1]; bool add_pa=a[0]==0u; bool add_pb=b[0]==0u; if (add_pa==add_pb) {uint add_carry=0u; for(int add_i=PRECISION; add_i>0; add_i--) {uint add_next=0u; if(a[add_i]+b[add_i]<a[add_i]) {add_next=1u;} add_buffer[add_i]=a[add_i]+b[add_i]+add_carry; add_carry=add_next;} if(!add_pa) {add_buffer[0]=1u;} else {add_buffer[0]=0u;}} else {bool add_flip=false; for(int add_i=1; add_i<=PRECISION; add_i++) {if(b[add_i]>a[add_i]) {add_flip=true; break;} if(a[add_i]>b[add_i]) {break;}} if(add_flip) {uint add_borrow=0u; for(int add_i=PRECISION; add_i>0; add_i--) {add_buffer[add_i]=b[add_i]-a[add_i]-add_borrow; if(b[add_i]<a[add_i]+add_borrow) {add_borrow=1u;} else {add_borrow=0u;}}} else {uint add_borrow=0u; for(int add_i=PRECISION; add_i>0; add_i--) {add_buffer[add_i]=a[add_i]-b[add_i]-add_borrow; if(a[add_i]<b[add_i]||a[add_i]<b[add_i]+add_borrow) {add_borrow=1u;} else {add_borrow=0u;}}} if(add_pa==add_flip) {add_buffer[0]=1u;} else {add_buffer[0]=0u;}} assign(r, add_buffer);}
#define mul(a, b, r) {uint mul_buffer[PRECISION+1]; zero(mul_buffer); uint mul_product[2*PRECISION-1]; for(int mul_i=0; mul_i<2*PRECISION-1; mul_i++) {mul_product[mul_i]=0u;} for(int mul_i=0; mul_i<PRECISION; mul_i++) {uint mul_carry=0u; for(int mul_j=0; mul_j<PRECISION; mul_j++) {uint mul_next=0; uint mul_value=a[PRECISION-mul_i]*b[PRECISION-mul_j]; if(mul_product[mul_i+mul_j]+mul_value<mul_product[mul_i+mul_j]) {mul_next++;} mul_product[mul_i+mul_j]+=mul_value; if(mul_product[mul_i+mul_j]+mul_carry<mul_product[mul_i+mul_j]) {mul_next++;} mul_product[mul_i+mul_j]+=mul_carry; uint mul_lower_a=a[PRECISION-mul_i]&0xFFFF; uint mul_upper_a=a[PRECISION-mul_i]>>16; uint mul_lower_b=b[PRECISION-mul_j]&0xFFFF; uint mul_upper_b=b[PRECISION-mul_j]>>16; uint mul_lower=mul_lower_a*mul_lower_b; uint mul_upper=mul_upper_a*mul_upper_b; uint mul_mid=mul_lower_a*mul_upper_b; mul_upper+=mul_mid>>16; mul_mid=mul_mid<<16; if(mul_lower+mul_mid<mul_lower) {mul_upper++;} mul_lower+=mul_mid; mul_mid=mul_lower_b*mul_upper_a; mul_upper+=mul_mid>>16; mul_mid=mul_mid<<16; if(mul_lower+mul_mid<mul_lower) {mul_upper++;}; mul_carry=mul_upper+mul_next;} if(mul_i+PRECISION<2*PRECISION-1) {mul_product[mul_i+PRECISION]+=mul_carry;}} if(mul_product[PRECISION-2]>=HALF_BASE) {for(int mul_i=PRECISION-1; mul_i<2*PRECISION-1; mul_i++) {if(mul_product[mul_i]+1>mul_product[mul_i]) {mul_product[mul_i]++; break;} mul_product[mul_i]++;}} for(int mul_i=0; mul_i<PRECISION; mul_i++) {mul_buffer[mul_i+1]=mul_product[2*PRECISION-2-mul_i];} if((a[0]==0u)!=(b[0]==0u)) {mul_buffer[0]=1u;}; assign(r, mul_buffer);}
)");

}

Arbitrary::Arbitrary() {
    this->load(0.0f);
}

Arbitrary::Arbitrary(double value) {
    this->load(value);
}

Arbitrary::Arbitrary(std::string value) {
    this->load(value);
}

void Arbitrary::load(double value) {

    // Reset the number.
    for (int i = 0; i <= PRECISION; i++) {this->values.push_back(0);}

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

void Arbitrary::load(const std::string value) {

    // Check if the string is of correct format.
    if (!Arbitrary::validate(value)) {throw std::invalid_argument("Invalid string.");}

    // Copy the string.
    std::string copy = value;

    // Reset the number.
    for (int i = 0; i <= PRECISION; i++) {this->values.push_back(0);}

    // Negate if required.
    if (copy[0] == '-') {
        this->values[0] = 1;
        copy.erase(0, 1);
    }

    // Split the string into both component strings.
    char delimiter = '.';
    std::stringstream ss(copy);
    std::string token;
    std::vector<std::string> tokens;
    while (std::getline(ss, token, delimiter)) {
        tokens.push_back(token);
    }

    // Compute the integral component.
    this->values[1] = std::stoul(tokens[0]);

    // Check if there is a fractional component.
    if (tokens.size() < 2) {return;}

    // Load the string into a vector of ints.
    std::vector<char> digits;
    for (char& c : tokens[1]) {
        digits.push_back(c-48);
    }

    // Compute the fractional component.
    for (int i = 2; i <= PRECISION ; i++) {

        uint32_t component = 0;      
        uint32_t mask = 1 << 31;

        for (int j = 0; j < 32; j++) {

            // See if there is any carry in the first element.
            if (digits[0] >= 5) {component = component | mask;}
            mask = mask >> 1;

            // Multiple the digit vector by two.
            for (int k = 0; k < digits.size(); k++) {
                if (k > 0 && digits[k] >= 5) {digits[k-1]++;}
                digits[k] = (digits[k] * 2) % 10;
            }

        }

        this->values[i] = component;
    }

}

uint32_t* Arbitrary::data() {
    return this->values.data();
}

int Arbitrary::precision() {
    return PRECISION;
}

std::string Arbitrary::glsl() {
    return GLSL;
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

std::string Arbitrary::serialise(const Arbitrary n) {
    
    bool sign = n.values[0] > 0;
    uint32_t integral = n.values[1];

    std::string result = "";
    if (sign) {result = "-" + result;}
    result = result + std::to_string(integral) + ".";

    std::vector<int> fractional;
    int length = std::ceil((PRECISION-1) * std::log10(BASE));
    for (int i = 0; i < length; i++) {fractional.push_back(0);}

    for (int i = PRECISION; i > 1; i--) {

        uint32_t component = n.values[i];        
        for (int j = 0; j < 32; j++) {

            // Divide the number by 2.
            int carry = 0;
            for (int k = 0; k < length; k++) {
                int next = (fractional[k] & 1) == 0 ? 0 : 1;
                fractional[k] = (10 * carry + fractional[k]) / 2;
                carry = next;
            }

            // Add the least significant bit to the number.
            carry = component & 1;
            for (int k = 0; k < length && carry != 0; k++) {
                fractional[k] += carry;
                carry = fractional[k] / 10;
                fractional[k] = fractional[k] % 10;
            }

            // Find the next least significant bit.
            component = component >> 1;

        }

    }

    // Divide the number by 2.
    int carry = 0;
    for (int k = 0; k < length; k++) {
        int next = (fractional[k] & 1) == 0 ? 0 : 1;
        fractional[k] = (10 * carry + fractional[k]) / 2;
        carry = next;
    }

    // Add the integer array to the string.
    for (int k = 1; k < length; k++) {result += std::to_string(fractional[k]);}

    return result;

}

Arbitrary Arbitrary::parse(const std::string value) {
    Arbitrary result(value);
    return result;
}

Arbitrary Arbitrary::negate(const Arbitrary n) {
    Arbitrary result = n;
    if (result.values[0] > 0) {result.values[0] = 0;}
    else {result.values[0] = 1;}
    return result;
}

Arbitrary Arbitrary::reciprocal(const Arbitrary n) {

    Arbitrary y = n;
    Arbitrary z = n;

    bool negate = false;
    if (n.values[0] > 0) {
        negate = true;
        y.values[0] = 0;
        z.values[0] = 0;
    }

    for (int i = PRECISION; i > 1; i--) {z.values[i] = z.values[i-1];}
    z.values[1] = 0; 

    for (int i = 0; i < 100; i++) {
        z = z * (Arbitrary(2.0) - (y * z));
    }

    if (negate) {z.values[0] = 1;}
    return z;

}

bool Arbitrary::validate(const std::string value) {

    // Create a copy of the string to test on.
    std::string copy = value;

    // Do initial check of the string using a regex.
    int integral_length = std::ceil(std::log10(BASE));
    int fractional_length = std::ceil((PRECISION-1) * std::log10(BASE));
    std::regex pattern("^-?\\d{1," + std::to_string(integral_length) + "}(\\.\\d{0," + std::to_string(fractional_length) + "})?$");
    if (!std::regex_match(copy, pattern)) {return false;}
    
    // Check whether the integral component fits.

    // Disregard the negation.
    if (copy[0] == '-') {copy.erase(0, 1);}

    // Split the string into both component strings.
    char delimiter = '.';
    std::stringstream ss(copy);
    std::string token;
    std::vector<std::string> tokens;
    while (std::getline(ss, token, delimiter)) {tokens.push_back(token);}

    // Store the integer in an unsigned long long and check if it is larger than unsigned long max.
    if (std::stoull(tokens[0]) > std::numeric_limits<uint32_t>::max()) {return false;}

    return true;

}

int Arbitrary::max_length() {
    return 2 + std::ceil(std::log10(BASE)) + std::ceil((PRECISION-1) * std::log10(BASE));
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
            if (a.values[i] < b.values[i] || a.values[i] < b.values[i] + borrow) {borrow = 1;} 
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
            uint32_t upper_a = a.values[PRECISION-i] >> 16;
            uint32_t lower_b = b.values[PRECISION-j] & 0xFFFF;
            uint32_t upper_b = b.values[PRECISION-j] >> 16;
            uint32_t lower = lower_a * lower_b;
	        uint32_t upper = upper_a * upper_b;

            uint32_t t = lower_a * upper_b;
            upper += t >> 16;
            t = t << 16;
            if (lower + t < lower) {upper++;}
            lower += t;

            t = lower_b * upper_a;
            upper += t >> 16;
            t = t << 16; 
            if (lower + t < lower) {upper++;}

            carry = upper + next;

        }

        if (i+PRECISION < 2*PRECISION-1) {
            product[i+PRECISION] += carry;
        }
        
    }

    // Round to the specified precision.
    if (product[PRECISION-2] >= BASE/2) {
        for (int i = product[PRECISION-1]; i < product.size(); i++) {
            if (product[i] + 1 > product[i]) {product[i]++; break;}
            product[i]++; 
        }
    }

    // Move the product into the arbitrary class.
    for (int i = 0; i < PRECISION; i++) {result.values[i+1] = product[product.size()-i-1];}

    // Fix the sign.
    if ((a.values[0] == 0) != (b.values[0] == 0)) {result.values[0] = 1;}
    return result;

}

Arbitrary Arbitrary::operator/(const Arbitrary& other) {

    Arbitrary a = *this;
    Arbitrary b = Arbitrary::reciprocal(other);
    
    return a * b;

}
