#ifndef COMPLEX_VECTOR_H
#define COMPLEX_VECTOR_H

#include <stddef.h>
#include "complex.h"

/**
 * Struttura per rappresentare un vettore di numeri complessi.
 * data: array dinamico di strutture Complex.
 * size: numero di elementi (dimensione del vettore).
 */
typedef struct {
    Complex *data;
    size_t size; 
} ComplexVector;

/**
 * Alloca memoria per un vettore di numeri complessi.
 * Input: size (numero di elementi da allocare)
 * Output: Struttura ComplexVector inizializzata
 */
ComplexVector alloc_complex_vector(size_t size);

/**
 * Libera la memoria occupata dai dati del vettore.
 * Input: vector (puntatore alla struttura da deallocare)
 */
void free_complex_vector(ComplexVector* vector);

/**
 * Inizializza a zero (0.0 + i0.0) tutti gli elementi del vettore.
 * Input: vector (puntatore al vettore da azzerare)
 */
void zero_complex_vector(ComplexVector* vector);

/**
 * Stampa il contenuto del vettore in formato leggibile.
 * Input: vector (puntatore costante al vettore da stampare)
 */
void print_complex_vector(const ComplexVector* vector);

#endif
