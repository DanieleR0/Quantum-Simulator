#include <stdio.h>
#include <math.h>
#include "complex.h"

Complex complex_add(Complex a, Complex b) {
    Complex result;
    // Somma separata delle componenti reali e immaginarie
    result.real = a.real + b.real;
    result.imag = a.imag + b.imag;
    return result;
}

Complex complex_mul(Complex a, Complex b) {
    Complex result;
    // Formula: (a+ib)*(c+id) = (ac - bd) + i(ad + bc)
    result.real = (a.real * b.real) - (a.imag * b.imag);
    result.imag = (a.real * b.imag) + (a.imag * b.real);
    return result;
}

double complex_mod(Complex a) {
    // Calcolo della distanza dall'origine nel piano
    return sqrt((a.real * a.real) + (a.imag * a.imag));
}

void print_complex(Complex c) {
    // Gestione del segno della parte immaginaria
    if (c.imag < 0) {
        printf("%.5f - i%.5f", c.real, -c.imag);
    } else {
        printf("%.5f + i%.5f", c.real, c.imag);
    }
}

int complex_equal(Complex a, Complex b, double eps) {
    // Verifica se la differenza tra le componenti è entro la soglia di errore
    if (fabs(a.real - b.real) < eps && fabs(a.imag - b.imag) < eps) {
        return 1;
    }
    return 0;
}

