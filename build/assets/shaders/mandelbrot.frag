#version 330 core

#define BASE 10000
#define MAX_PRECISION 5
#define MAX_PRODUCT_SIZE ((MAX_PRECISION-1)*2+1)

in vec2 fPosition;
in vec2 fTexCoords;

uniform int uPrecision;
uniform int uIterations;

uniform int uReal[MAX_PRECISION];
uniform int uImaginary[MAX_PRECISION];
uniform int uWidth[MAX_PRECISION];
uniform int uHeight[MAX_PRECISION];
uniform int uDepth[MAX_PRECISION];

out vec4 colour;

int result[MAX_PRECISION];

#define zero(x, l) for(int i=0;i<l;i++){x[i]=0;}
#define assign(x, y) for(int i=0;i<uPrecision;i++){x[i]=y[i];}
#define negate(x) for(int i=0;i<uPrecision;i++){x[i]=-x[i];}
#define signp(x) (x[0] >= 0)

int keep, carry;

void round(int x) {
	carry = x / BASE;
	keep = x - carry * BASE;
}

void add(int[MAX_PRECISION] a, int[MAX_PRECISION] b) {

	bool s1 = signp(a);
    bool s2 = signp(b);
	carry = 0;

	for (int i = uPrecision - 1; i > 0; i--) {
		
        round(a[i] + b[i] + carry);
		if (keep < 0) {
			keep += BASE;
			carry--;
		}

		result[i] = keep;

	}

	round(a[0] + b[0] + carry);
	result[0] = keep;
	
	if (s1 != s2 && !signp(result)) {

		negate(result);
		carry = 0;

		for (int i = uPrecision - 1; i >= 0; i--) {
			
            round(result[i] + carry);
			if (keep < 0) {
				keep += BASE;
				carry--;
			}

			result[i] = keep;

		}

		negate(result);

	}

}

void mul(int[MAX_PRECISION] a, int[MAX_PRECISION] b) {

	bool flag = false;

	if (!signp(a)) {
		negate(a);
		flag = !flag;
	}

	if (!signp(b)) {
		negate(b);
		flag = !flag;
	}

    int len = ((uPrecision-1)*2+1);
	int prod[MAX_PRODUCT_SIZE];
	zero(prod, len);

	for (int i = 0; i < uPrecision; i++) {
		for(int j = 0; j < uPrecision; j++) {
			prod[i+j] += a[uPrecision-i-1] * b[uPrecision-j-1];
		}
	}

	carry = 0;
	int clip = len - uPrecision;
	for (int i = 0; i < clip; i++) {
		round(prod[i] + carry);
		prod[i] = keep;
	}

	if (prod[clip-1] >= BASE/2) {
		carry++;
	}

	for (int i = clip; i < len; i++) {
		round(prod[i] + carry);
		prod[i] = keep;
	}

	for (int i = 0; i < len - clip; i++) {
		result[uPrecision-i-1] = prod[i+clip];
	}

	if (flag) {
		negate(result);
	}

}

void load(float f) {

    for (int i = 0; i < uPrecision; i++) {
        int fCurr = int(f);
        result[i] = fCurr;
        f -= float(fCurr);
        f *= float(BASE);
    }

}

float mandelbrot(int[MAX_PRECISION] c_r, int[MAX_PRECISION] c_i) {

    int z_r[MAX_PRECISION];
    int z_i[MAX_PRECISION];
    zero(z_r, uPrecision);
    zero(z_i, uPrecision);
    
    int tmp[MAX_PRECISION];
    int nz_r[MAX_PRECISION];
    int nz_i[MAX_PRECISION];

    int k = 0;
    while (k < uIterations) {

        // Compute c_r^2 + c_i^2
        // Pythagoras Theorem
        int radius[MAX_PRECISION];
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

    if (k < uIterations) {
        return float(k) / float(uIterations);
    }

    return 0.0;
}

void main() {

    int tmp[MAX_PRECISION];
    load(0.5);
    assign(tmp, result);

    int c_r[MAX_PRECISION];
    load(fPosition.x);
    assign(c_r, result);
    mul(c_r, uWidth);
    assign(c_r, result);
    mul(c_r, uDepth);
    assign(c_r, result);
    mul(c_r, tmp);
    assign(c_r, result);
    add(c_r, uReal);
    assign(c_r, result);

    int c_i[MAX_PRECISION];
    load(fPosition.y);
    assign(c_i, result);
    mul(c_i, uHeight);
    assign(c_i, result);
    mul(c_i, uDepth);
    assign(c_i, result);
    mul(c_i, tmp);
    assign(c_i, result);
    add(c_i, uImaginary);
    assign(c_i, result);

    colour = vec4(vec3(1.0, 1.0, 1.0) * mandelbrot(c_r, c_i), 1.0);

}