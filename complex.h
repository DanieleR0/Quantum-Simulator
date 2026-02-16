#ifndef COMPLEX_H
#define COMPLEX_H

#define EPSILON 1e-9

/**
 * Struttura per rappresentare un numero complesso.
 * real: parte reale.
 * imag: parte immaginaria.
 */
typedef struct {
    double real;
    double imag;
} Complex;

/* Operazioni matematiche */

/**
 * Esegue la somma algebrica di due numeri complessi.
 * Input: a, b (Complex)
 * Output: Complex risultante
 */
Complex complex_add(Complex a, Complex b);

/**
 * Esegue il prodotto di due numeri complessi (regola del parallelogramma).
 * Input: a, b (Complex)
 * Output: Complex risultante
 */
Complex complex_mul(Complex a, Complex b);

/**
 * Calcola il modulo di un numero complesso.
 * Input: a (Complex)
 * Output: double (sqrt(re^2 + im^2))
 */
double complex_mod(Complex a);

/* Utility / debug */

/**
 * Stampa un numero complesso nel formato "a + ib".
 * Input: c (Complex)
 */
void print_complex(Complex c);

/**
 * Confronta due numeri complessi con una tolleranza epsilon.
 * Input: a, b (Complex), eps (double)
 * Output: 1 se uguali, 0 altrimenti
 */
int complex_equal(Complex a, Complex b, double eps); 

#endif
