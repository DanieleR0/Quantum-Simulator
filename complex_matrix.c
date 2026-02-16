#include <stdio.h>
#include <stdlib.h>
#include <string.h> // Per memcpy
#include "complex_matrix.h"

/* Macro per l'indicizzazione 2D in array 1D: (riga * num_colonne + colonna) */
#define MAT(m, i, j) ((m)->data[(i) * (m)->cols + (j)])

ComplexMatrix alloc_complex_matrix(size_t rows, size_t cols) {
    ComplexMatrix matrix;
    matrix.rows = rows;
    matrix.cols = cols;

    // calloc per azzerare la memoria (0.0 + i0.0)
    matrix.data = calloc(rows * cols, sizeof(Complex));
    
    if (!matrix.data) {
        fprintf(stderr, "FATAL: Out of memory allocating %zu x %zu matrix\n", rows, cols);
        exit(EXIT_FAILURE);
    }
    return matrix;
}

void free_complex_matrix(ComplexMatrix *matrix) {
    if (!matrix || !matrix->data) return;

    free(matrix->data);
    matrix->data = NULL;
    matrix->rows = 0;
    matrix->cols = 0;
}

ComplexMatrix copy_complex_matrix(const ComplexMatrix *src) {
    ComplexMatrix copy = alloc_complex_matrix(src->rows, src->cols);

    // aggiornato rispetto a ciclo for
    memcpy(copy.data, src->data, src->rows * src->cols * sizeof(Complex));

    return copy;
}

ComplexMatrix matrix_mul(const ComplexMatrix *a, const ComplexMatrix *b) {
    // Controllo coerenza dimensionale per prodotto matriciale
    if (a->cols != b->rows) {
        fprintf(stderr, "Error: incompatible matrix dimensions\n");
        exit(EXIT_FAILURE);
    }

    ComplexMatrix result = alloc_complex_matrix(a->rows, b->cols);

    for (size_t i = 0; i < a->rows; i++) {
        for (size_t j = 0; j < b->cols; j++) {
            Complex sum = {0.0, 0.0};
            for (size_t k = 0; k < a->cols; k++) {
                // Accumulo prodotto riga per colonna
                Complex prod = complex_mul(MAT(a, i, k), MAT(b, k, j));
                sum = complex_add(sum, prod);
            }
            MAT(&result, i, j) = sum;
        }
    }
    return result;
}

ComplexVector matrix_vector_mul(const ComplexMatrix *a, const ComplexVector *v) {
    // Controllo che il numero di colonne della matrice sia uguale alla dimensione del vettore
    if (a->cols != v->size) {
        fprintf(stderr, "Error: incompatible matrix/vector dimensions\n");
        exit(EXIT_FAILURE);
    }

    ComplexVector result = alloc_complex_vector(a->rows);

    for (size_t i = 0; i < a->rows; i++) {
        Complex sum = {0.0, 0.0};
        for (size_t j = 0; j < a->cols; j++) {
            // Calcolo elemento i-esimo del vettore risultante
            Complex prod = complex_mul(MAT(a, i, j), v->data[j]);
            sum = complex_add(sum, prod);
        }
        result.data[i] = sum;
    }
    return result;
}

void print_complex_matrix(const ComplexMatrix *matrix) {
    for (size_t i = 0; i < matrix->rows; i++) {
        printf("[ ");
        for (size_t j = 0; j < matrix->cols; j++) {
            print_complex(matrix->data[i * matrix->cols + j]);
            printf(" ");
        }
        printf("]\n");
    }
}