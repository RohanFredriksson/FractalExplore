#version 400 core

#define BASE 4294967296.0
#define PRECISION 2

in vec2 fPosition;
in vec2 fTexCoords;

uniform int uIterations;
uniform uint uReal[PRECISION+1];
uniform uint uImaginary[PRECISION+1];
uniform uint uWidth[PRECISION+1];
uniform uint uHeight[PRECISION+1];
uniform uint uDepth[PRECISION+1];

out vec4 colour;

uint carry(uint a, uint b) {

    uint lower_a = a & 0xFFFF;
    uint upper_a = a >> 16;
    uint lower_b = b & 0xFFFF;
    uint upper_b = b >> 16;
    uint lower = lower_a * lower_b;
    uint upper = upper_a * upper_b;

    uint mid = lower_a * upper_b;
    upper += mid >> 16;
    mid = mid << 16;
    if (lower + mid < lower) {upper++;}
    lower += mid;

    mid = lower_b * upper_a;
    upper += mid >> 16;
    mid = mid << 16; 
    if (lower + mid < lower) {upper++;}

    return upper;

}

#define assign(x, y) {for(int assign_i=0;assign_i<=PRECISION;assign_i++){x[assign_i]=y[assign_i];}}
#define zero(x) {for(int zero_i=0;zero_i<=PRECISION;zero_i++){x[zero_i]=0u;}}
#define load(x, v) {double f=(v); if (f<0.0) {x[0]=1u; f*=-1.0;} else {x[0]=0u;} for(int load_i=1; load_i<=PRECISION; load_i++) {x[load_i]=uint(f); f-=x[load_i]; f*=BASE;}}
#define shift(x, v) {int shift_n=(v); for(int shift_i=shift_n+1; shift_i<=PRECISION; shift_i++) {x[shift_i]=x[shift_i-shift_n];} for(int shift_i=1; shift_i<=shift_n; shift_i++) {x[shift_i]=0u;}};
#define negate(x) {x[0]=(x[0]==0u?1u:0u);}

#define add_add(a, b) {uint add_carry=0u; for(int add_i=PRECISION; add_i>0; add_i--) {uint add_next=0u; if(a[add_i]+b[add_i]<a[add_i]) {add_next=1u;} add_buffer[add_i]=a[add_i]+b[add_i]+add_carry; add_carry=add_next;} if(!add_pa) {add_buffer[0]=1u;} else {add_buffer[0]=0u;}}
#define add_sub(a, b) {uint add_borrow=0u; for(int add_i=PRECISION; add_i>0; add_i--) {add_buffer[add_i]=a[add_i]-b[add_i]-add_borrow; if(a[add_i]<b[add_i]+add_borrow) {add_borrow=1u;} else {add_borrow=0u;}}}
#define add(a, b, r) {uint add_buffer[PRECISION+1]; bool add_pa=a[0]==0u; bool add_pb=b[0]==0u; if (add_pa==add_pb) {add_add(a, b);} else {bool add_flip=false; for(int add_i=1; add_i<=PRECISION; add_i++) {if(b[add_i]>a[add_i]) {add_flip=true; break;} if(a[add_i]>b[add_i]) {break;}} if(add_flip) {add_sub(b, a);} else {add_sub(a, b);} if(add_pa==add_flip) {add_buffer[0]=1u;} else {add_buffer[0]=0u;}} assign(r, add_buffer);}
#define mul(a, b, r) {uint mul_buffer[PRECISION+1]; zero(mul_buffer); for(int mul_i=0; mul_i<PRECISION; mul_i++) {uint mul_partial[PRECISION+1]; zero(mul_partial); uint mul_carry = 0u; for (int mul_j=0; mul_j<PRECISION; mul_j++) {uint mul_next=0u; mul_partial[PRECISION-mul_j]=a[mul_i+1]*b[PRECISION-mul_j]; if (mul_partial[PRECISION-mul_j]+mul_carry<mul_partial[PRECISION-mul_j]) {mul_next++;} mul_partial[PRECISION-mul_j]+=mul_carry; mul_next+=carry(a[mul_i+1], b[PRECISION-mul_j]); mul_carry=mul_next;} if(mul_i>0) {shift(mul_partial, mul_i); mul_partial[mul_i]=mul_carry;} add(mul_buffer, mul_partial, mul_buffer);} if((a[0]==0u)!=(b[0]==0u)) {mul_buffer[0]=1u;}; assign(r, mul_buffer);}

float mandelbrot(uint[PRECISION+1] c_r, uint[PRECISION+1] c_i) {

    uint z_r[PRECISION+1];
    uint z_i[PRECISION+1];
    zero(z_r);
    zero(z_i);
    
    uint tmp[PRECISION+1];
    uint nz_r[PRECISION+1];
    uint nz_i[PRECISION+1];

    int k = 0;
    while (k < uIterations) {

        // Compute c_r^2 + c_i^2
        // Pythagoras Theorem
        uint radius[PRECISION+1];
        mul(z_r, z_r, radius);
        mul(z_i, z_i, tmp);
        add(radius, tmp, radius);

        // If we are more than 2 units away, c_r^2 + c_i^2 will be greater than 4.
        if (radius[1] > 4) {
            break;
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

        // Increment the counter.
        k++;

    }

    if (k < uIterations) {
        return float(k) / float(uIterations);
    }

    return 0.0;
}

void main() {

    uint half[PRECISION+1];
    load(half, 0.5);

    uint c_r[PRECISION+1];
    load(c_r, fPosition.x);
    mul(c_r, uWidth, c_r);
    mul(c_r, uDepth, c_r);
    mul(c_r, half, c_r);
    add(c_r, uReal, c_r);

    uint c_i[PRECISION+1];
    load(c_i, fPosition.y);
    mul(c_i, uHeight, c_i);
    mul(c_i, uDepth, c_i);
    mul(c_i, half, c_i);
    add(c_i, uImaginary, c_i);
    
    colour = vec4(vec3(1.0, 1.0, 1.0) * mandelbrot(c_r, c_i), 1.0);

}