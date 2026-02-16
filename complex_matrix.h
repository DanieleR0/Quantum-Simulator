#ifndef COMPLEX_MATRIX_H
#define COMPLEX_MATRIX_H

#include <stddef.h>
#include "complex.h"
#include "complex_vector.h"

/**
 * Struttura per rappresentare una matrice di numeri complessi.
 * rows, cols: dimensioni della matrice.
 * data: puntatore all'array contiguo di elementi Complex.
 */
typedef struct {
    size_t rows;
    size_t cols;
    Complex *data;
} ComplexMatrix;

/**
 * Alloca memoria per una matrice di dimensioni specificate.
 * Input: rows, cols (size_t)
 * Output: ComplexMatrix inizializzata a zero
 */
ComplexMatrix alloc_complex_matrix(size_t rows, size_t cols);

/**
 * Libera la memoria allocata per la matrice.
 * Input: matrix (puntatore a ComplexMatrix)
 */
void free_complex_matrix(ComplexMatrix* matrix);

/* Operazioni Matematiche */

/**
 * Esegue il prodotto tra due matrici (righe per colonne).
 * Input: a, b (puntatori a ComplexMatrix costanti)
 * Output: ComplexMatrix risultato
 */
ComplexMatrix matrix_mul(const ComplexMatrix *a, const ComplexMatrix *b);

/**
 * Moltiplica una matrice per un vettore colonna.
 * Input: a (matrice), v (vettore)
 * Output: ComplexVector risultato
 */
ComplexVector matrix_vector_mul(const ComplexMatrix *a, const ComplexVector *v);

/**
 * Crea una copia esatta della matrice sorgente in una nuova zona di memoria.
 * Input: src (matrice sorgente)
 * Output: ComplexMatrix (nuova copia)
 */
ComplexMatrix copy_complex_matrix(const ComplexMatrix *src);

/**
 * Stampa la matrice a terminale (debug).
 * Input: matrix (puntatore a ComplexMatrix costante)
 */
void print_complex_matrix(const ComplexMatrix *matrix);

#endif