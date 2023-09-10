#include "fractal.hpp"

namespace BurningShip {

const char* vertex = R"(#version 400 core

layout (location=0) in vec2 aPosition;
layout (location=1) in vec2 aTexCoords;

out vec2 fPosition;
out vec2 fTexCoords;

void main() {
    gl_Position = vec4(aPosition, 0.0, 1.0);
    fPosition = aPosition;
    fTexCoords = aTexCoords;
})";

const char* fragment = R"(#version 400 core

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
#define add(a, b, r) {uint add_buffer[PRECISION+1]; bool add_pa=a[0]==0u; bool add_pb=b[0]==0u; if (add_pa==add_pb) {uint add_carry=0u; for(int add_i=PRECISION; add_i>0; add_i--) {uint add_next=0u; if(a[add_i]+b[add_i]<a[add_i]) {add_next=1u;} add_buffer[add_i]=a[add_i]+b[add_i]+add_carry; add_carry=add_next;} if(!add_pa) {add_buffer[0]=1u;} else {add_buffer[0]=0u;}} else {bool add_flip=false; for(int add_i=1; add_i<=PRECISION; add_i++) {if(b[add_i]>a[add_i]) {add_flip=true; break;} if(a[add_i]>b[add_i]) {break;}} if(add_flip) {uint add_borrow=0u; for(int add_i=PRECISION; add_i>0; add_i--) {add_buffer[add_i]=b[add_i]-a[add_i]-add_borrow; if(b[add_i]<a[add_i]+add_borrow) {add_borrow=1u;} else {add_borrow=0u;}}} else {uint add_borrow=0u; for(int add_i=PRECISION; add_i>0; add_i--) {add_buffer[add_i]=a[add_i]-b[add_i]-add_borrow; if(a[add_i]<b[add_i]+add_borrow) {add_borrow=1u;} else {add_borrow=0u;}}} if(add_pa==add_flip) {add_buffer[0]=1u;} else {add_buffer[0]=0u;}} assign(r, add_buffer);}
#define mul(a, b, r) {uint mul_buffer[PRECISION+1]; zero(mul_buffer); uint mul_product[2*PRECISION-1]; for(int mul_i=0; mul_i<2*PRECISION-1; mul_i++) {mul_product[mul_i]=0u;} for(int mul_i=0; mul_i<PRECISION; mul_i++) {uint mul_carry=0u; for(int mul_j=0; mul_j<PRECISION; mul_j++) {uint mul_next=0; uint mul_value=a[PRECISION-mul_i]*b[PRECISION-mul_j]; if(mul_product[mul_i+mul_j]+mul_value<mul_product[mul_i+mul_j]) {mul_next++;} mul_product[mul_i+mul_j]+=mul_value; if(mul_product[mul_i+mul_j]+mul_carry<mul_product[mul_i+mul_j]) {mul_next++;} mul_product[mul_i+mul_j]+=mul_carry; uint mul_lower_a=a[PRECISION-mul_i]&0xFFFF; uint mul_upper_a=a[PRECISION-mul_i]>>16; uint mul_lower_b=b[PRECISION-mul_j]&0xFFFF; uint mul_upper_b=b[PRECISION-mul_j]>>16; uint mul_lower=mul_lower_a*mul_lower_b; uint mul_upper=mul_upper_a*mul_upper_b; uint mul_mid=mul_lower_a*mul_upper_b; mul_upper+=mul_mid>>16; mul_mid=mul_mid<<16; if(mul_lower+mul_mid<mul_lower) {mul_upper++;} mul_lower+=mul_mid; mul_mid=mul_lower_b*mul_upper_a; mul_upper+=mul_mid>>16; mul_mid=mul_mid<<16; if(mul_lower+mul_mid<mul_lower) {mul_upper++;}; mul_carry=mul_upper+mul_next;} if(mul_i+PRECISION<2*PRECISION-1) {mul_product[mul_i+PRECISION]+=mul_carry;}} if(mul_product[PRECISION-2]>=HALF_BASE) {for(int mul_i=PRECISION-1; mul_i<2*PRECISION-1; mul_i++) {if(mul_product[mul_i]+1>mul_product[mul_i]) {mul_product[mul_i]++; break;} mul_product[mul_i]++;}} for(int mul_i=0; mul_i<PRECISION; mul_i++) {mul_buffer[mul_i+1]=mul_product[2*PRECISION-2-mul_i];} if((a[0]==0u)!=(b[0]==0u)) {mul_buffer[0]=1u;}; assign(r, mul_buffer);}

in vec2 fPosition;
in vec2 fTexCoords;

uniform int uIterations;
uniform uint uPositionX[ARRAY_SIZE];
uniform uint uPositionY[ARRAY_SIZE];
uniform uint uScaleX[ARRAY_SIZE];
uniform uint uScaleY[ARRAY_SIZE];

out vec4 colour;

float burningship(uint[ARRAY_SIZE] c_r, uint[ARRAY_SIZE] c_i) {

    uint z_r[ARRAY_SIZE];
    uint z_i[ARRAY_SIZE];
    zero(z_r);
    zero(z_i);
    
    uint tmp[ARRAY_SIZE];
    uint nz_r[ARRAY_SIZE];
    uint nz_i[ARRAY_SIZE];

    for (int k = 0; k < uIterations; k++) {

        // Take the absolute of each component.
        abs(z_r);
        abs(z_i);

        // Compute c_r^2 + c_i^2
        uint radius[ARRAY_SIZE];
        mul(z_r, z_r, radius);
        mul(z_i, z_i, tmp);
        add(radius, tmp, radius);

        // If we are more than 2 units away, c_r^2 + c_i^2 will be greater than 4.
        if (radius[1] > 4) {
            return float(k) / float(uIterations);
        }

        // Compute the real component of the square.
        mul(z_i, z_i, nz_r);
        negate(nz_r);
        mul(z_r, z_r, tmp);
        add(nz_r, tmp, nz_r);
        add(nz_r, c_r, nz_r);

        // Compute the imaginary component of the square.
        load(nz_i, 2.0);
        mul(nz_i, z_r, nz_i);
        mul(nz_i, z_i, nz_i);
        add(nz_i, c_i, nz_i);

        // Update the past values.
        assign(z_r, nz_r);
        assign(z_i, nz_i);

    }

    return 0.0;
}

void main() {

    uint c_r[ARRAY_SIZE];
    load(c_r, fPosition.x);
    mul(c_r, uScaleX, c_r);
    add(c_r, uPositionX, c_r);

    uint c_i[ARRAY_SIZE];
    load(c_i, fPosition.y);
    mul(c_i, uScaleY, c_i);
    add(c_i, uPositionY, c_i);
    
    colour = vec4(vec3(1.0, 1.0, 1.0) * burningship(c_r, c_i), 1.0);

})";

}

REGISTER_FRACTAL(BurningShip, BurningShip::vertex, BurningShip::fragment);