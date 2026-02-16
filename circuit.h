#ifndef CIRCUIT_H
#define CIRCUIT_H

#include <stddef.h>
#include "complex_vector.h"
#include "complex_matrix.h"


// Macro per accedere agli elementi di una matrice complessa
#define MAT(m, i, j) ((m)->data[(i) * (m)->cols + (j)])


/*
 * Rappresenta una porta quantistica:
 * - name   : nome simbolico del gate
 * - matrix : matrice complessa 2^n x 2^n associata al gate
 */
typedef struct {
    char *name;
    ComplexMatrix matrix;
} Gate;

/*
 * Rappresenta un circuito quantistico a n qubits.
 */
typedef struct {
    unsigned int n_qubits;   /* numero di qubits */
    size_t dim;              /* dimensione spazio: 2^n */

    ComplexVector state;     /* stato corrente */

    Gate *gates;             /* array dei gate definiti */
    size_t gate_count;

    size_t *sequence;        /* sequenza di applicazione */
    size_t sequence_len;
} Circuit;

/* Inizializzazione e gestione */
void circuit_init(Circuit *c, unsigned int n_qubits);
void circuit_add_gate(Circuit *c, const char *name, ComplexMatrix matrix);
void circuit_set_sequence(Circuit *c, const size_t *sequence, size_t length);
void circuit_execute_parallel(Circuit *c, size_t n_threads);
void circuit_free(Circuit *c);

/* Debug / Output */
void circuit_print_state(const Circuit *c);
void circuit_print_gate(const Circuit *c, size_t gate_index);
void circuit_print_all_gates(const Circuit *c);

#endif

