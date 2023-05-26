#version 330 core

#define BASE 10000
#define PRECISION 4
#define ITERATIONS 128

in vec2 fPosition;
in vec2 fTexCoords;

uniform int ux[PRECISION];
uniform int uy[PRECISION];
uniform int uw[PRECISION];
uniform int uh[PRECISION];
uniform int ud[PRECISION];

out vec4 colour;

int result[PRECISION];

#define zero(x, len) for(int i=0;i<len;i++){x[i]=0;}
#define assign(x, y) for(int i=0;i<PRECISION;i++){x[i]=y[i];}
#define negate(x) for(int i=0;i<PRECISION;i++){x[i]=-x[i];}

bool signp(int[PRECISION] a) {
	return (a[PRECISION-1] >= 0);
}

int keepVal, carry;

void roundOff(int x) {
	carry = x / BASE;
	keepVal = x - carry * BASE;
}

void add(int[PRECISION] a, int[PRECISION] b) {

	bool s1 = signp(a), s2 = signp(b);
	carry = 0;

	for (int i = 0; i < PRECISION-1; i++) {
		roundOff(a[i] + b[i] + carry);

		if (keepVal < 0) {
			keepVal += BASE;
			carry--;
		}

		result[i] = keepVal;
	}

	roundOff(a[PRECISION-1] + b[PRECISION-1] + carry);
	result[PRECISION-1] = keepVal;
	
	if (s1 != s2 && !signp(result)) {
		negate(result);

		carry = 0;

		for (int i = 0; i < PRECISION; i++) {
			roundOff(result[i] + carry);

			if (keepVal < 0) {
				keepVal += BASE;
				carry--;
			}

			result[i] = keepVal;
		}

		negate(result);
	}

}

void mul(int[PRECISION] a, int[PRECISION] b) {

	bool toNegate = false;

	if (!signp(a)) {
		negate(a);
		toNegate = !toNegate;
	}

	if (!signp(b)) {
		negate(b);
		toNegate = !toNegate;
	}

	const int lenProd = (PRECISION-1)*2+1;
	int prod[lenProd];
	zero(prod, lenProd);

	for (int i = 0; i < PRECISION; i++) {
		for(int j = 0; j < PRECISION; j++) {
			prod[i+j] += a[i] * b[j];
		}
	}

	carry = 0;
	const int clip = lenProd - PRECISION;
	for (int i = 0; i < clip; i++) {
		roundOff(prod[i] + carry);
		prod[i] = keepVal;
	}

	if (prod[clip-1] >= BASE/2) {
		carry++;
	}

	for (int i = clip; i < lenProd; i++) {
		roundOff(prod[i] + carry);
		prod[i] = keepVal;
	}

	for (int i = 0; i < lenProd - clip; i++) {
		result[i] = prod[i+clip];
	}

	if (toNegate) {
		negate(result);
	}

}

void load(float f) {
	for(int i = PRECISION - 1; i >= 0; i--) {
		int fCurr = int(f);
		result[i] = fCurr;
		f -= float(fCurr);
		f *= float(BASE);
	}
}

float mandelbrot(int[PRECISION] c_r, int[PRECISION] c_i) {

    int z_r[PRECISION];
    int z_i[PRECISION];
    zero(z_r, PRECISION);
    zero(z_i, PRECISION);
    
    int tmp[PRECISION];
    int nz_r[PRECISION];
    int nz_i[PRECISION];

    int k = 0;
    while (k < ITERATIONS) {

        // Compute c_r^2 + c_i^2
        // Pythagoras Theorem
        int radius[PRECISION];
        mul(z_r, z_r);
        assign(radius, result);
        mul(z_i, z_i);
        assign(tmp, result);
        add(radius, tmp);
        assign(radius, result);

        // If we are more than 2 units away, c_r^2 + c_i^2 will be greater than 4.
        // So if the result is negative, we keep iterating, otherwise we can bail.
        load(-4.0);
        assign(tmp, result);
        add(radius, tmp);
        assign(radius, result);
        if (signp(radius)) {
            break;
        }

        // Compute the real component of the square.
        mul(z_i, z_i);
        assign(nz_r, result);
        negate(nz_r);
        mul(z_r, z_r);
        assign(tmp, result);
        add(nz_r, tmp);
        assign(nz_r, result);
        add(nz_r, c_r);
        assign(nz_r, result);

        // Compute the imaginary component of the square.
        load(2.0);
        assign(nz_i, result);
        mul(nz_i, z_r);
        assign(nz_i, result);
        mul(nz_i, z_i);
        assign(nz_i, result);
        add(nz_i, c_i);
        assign(nz_i, result);

        // Update the past values.
        assign(z_r, nz_r);
        assign(z_i, nz_i);

        // Increment the counter.
        k++;

    }

    if (k < ITERATIONS) {
        return float(k) / float(ITERATIONS);
    }

    return 0.0;
}

void main() {

    int tmp[PRECISION];
    load(0.5);
    assign(tmp, result);

    int c_r[PRECISION];
    load(fPosition.x);
    assign(c_r, result);
    mul(c_r, uw);
    assign(c_r, result);
    mul(c_r, ud);
    assign(c_r, result);
    mul(c_r, tmp);
    assign(c_r, result);
    add(c_r, ux);
    assign(c_r, result);

    int c_i[PRECISION];
    load(fPosition.y);
    assign(c_i, result);
    mul(c_i, uh);
    assign(c_i, result);
    mul(c_i, ud);
    assign(c_i, result);
    mul(c_i, tmp);
    assign(c_i, result);
    add(c_i, uy);
    assign(c_i, result);

    colour = vec4(vec3(1.0, 1.0, 1.0) * mandelbrot(c_r, c_i), 1.0);

}