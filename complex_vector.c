#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "complex_vector.h"
#include "complex.h"

ComplexVector alloc_complex_vector(size_t size) {
    ComplexVector vector;
    vector.size = size;
    // Allocazione dinamica nello heap per gestire grandi dimensioni
    vector.data = malloc(sizeof(Complex) * size);
    
    if (vector.data == NULL) {
        fprintf(stderr, "Error: malloc failed for vector size %zu\n", size);
        exit(EXIT_FAILURE);
    }

    return vector;
}

void free_complex_vector(ComplexVector* vector) {
    if (vector == NULL || vector->data == NULL) return;

    free(vector->data);
    // Reset dei campi per evitare l'uso di memoria già liberata 
    vector->data = NULL;
    vector->size = 0;
}

void zero_complex_vector(ComplexVector* vector) {
    if (vector == NULL || vector->data == NULL) return;

    // aggiornato a memset 
    memset(vector->data, 0, vector->size * sizeof(Complex));
}

void print_complex_vector(const ComplexVector* vector) {
    if (!vector || !vector->data) {
        printf("[]\n");
        return;
    }

    printf("[");
    for (size_t i = 0; i < vector->size; i++) {
        print_complex(vector->data[i]);
        // Aggiunge la virgola solo tra gli elementi, non dopo l'ultimo
        if (i + 1 < vector->size)
            printf(", ");
    }
    printf("]\n");
}
