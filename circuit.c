#define _POSIX_C_SOURCE 200809L
#include "circuit.h"
#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>


/* STRUTTURE PER I THREAD */

/**
 * Struttura ThreadApplyTask: contiene i dati necessari a ogni thread
 * per calcolare una porzione del prodotto matrice-vettore.
 */
typedef struct {
    const ComplexMatrix *matrix;   // Matrice dell'operatore (gate)
    const ComplexVector *input;    // Stato del sistema in ingresso
    ComplexVector *output;         // Buffer per il nuovo stato calcolato
    size_t start;                  // Indice della prima riga da elaborare
    size_t end;                    // Indice dell'ultima riga (esclusa)
} ThreadApplyTask;


/* FUNZIONI THREAD */

/**
 * Funzione eseguita dal thread: calcola gli elementi del vettore di stato
 * nel range [start, end) moltiplicando le righe della matrice per il vettore.
 */
static void *thread_apply_matrix(void *arg) {
    ThreadApplyTask *task = (ThreadApplyTask *)arg;

    for (size_t i = task->start; i < task->end; i++) {
        Complex sum = {0.0, 0.0};

        // Prodotto riga per colonna: sum = Σ (matrix[i][j] * input[j])
        for (size_t j = 0; j < task->matrix->cols; j++) {
            Complex prod = complex_mul(MAT(task->matrix, i, j),
                                       task->input->data[j]);
            sum = complex_add(sum, prod);
        }

        task->output->data[i] = sum;
    }

    return NULL;
}


/* INIZIALIZZAZIONE CIRCUITO */

/**
 * Inizializza la struttura del circuito, imposta il numero di qubit
 * e alloca il vettore di stato iniziale.
 */
void circuit_init(Circuit *c, unsigned int n_qubits) {
    c->n_qubits = n_qubits;
    // La dimensione del vettore è 2^n_qubits
    c->dim = (size_t)1 << n_qubits; 

    c->state = alloc_complex_vector(c->dim);
    c->gates = NULL;
    c->gate_count = 0;
    c->sequence = NULL;
    c->sequence_len = 0;
}


/* AGGIUNTA DI UN GATE */

/**
 * Registra un nuovo gate nel dizionario dei gate del circuito.
 * Esegue una riallocazione dinamica per aggiungere spazio al nuovo gate.
 */
void circuit_add_gate(Circuit *c, const char *name, ComplexMatrix matrix) {
    c->gates = realloc(c->gates, (c->gate_count + 1) * sizeof(Gate));
    if (!c->gates) {
        perror("Errore realloc gates");
        exit(EXIT_FAILURE);
    }

    // Copia il nome del gate e assegna la matrice associata
    c->gates[c->gate_count].name = strdup(name);
    c->gates[c->gate_count].matrix = matrix;
    c->gate_count++;
}


/* DEFINIZIONE SEQUENZA GATE */

/**
 * Definisce l'ordine di applicazione dei gate nello spazio temporale del circuito.
 */
void circuit_set_sequence(Circuit *c, const size_t *sequence, size_t length) {
    c->sequence = malloc(length * sizeof(size_t));
    if (!c->sequence) {
        perror("Errore malloc sequence");
        exit(EXIT_FAILURE);
    }

    memcpy(c->sequence, sequence, length * sizeof(size_t));
    c->sequence_len = length;
}


/* ESECUZIONE CIRCUITO (PARALLELA) */

/**
 * Esegue la simulazione applicando sequenzialmente i gate allo stato.
 * La parallelizzazione avviene dividendo il carico di lavoro del prodotto 
 * matrice-vettore tra il numero di thread specificato.
 */
void circuit_execute_parallel(Circuit *c, size_t n_threads) {
    if (c->sequence_len == 0) return;

    size_t dim = c->dim;
    // Buffer ausiliario per memorizzare il risultato dell'applicazione di un gate
    ComplexVector intermediate_state = alloc_complex_vector(dim);
    
    // Puntatori per gestire lo scambio (swap) tra stato corrente e risultato
    ComplexVector *current_in = &(c->state);
    ComplexVector *current_out = &intermediate_state;

    // Itera attraverso ogni gate presente nella sequenza #circ
    for (size_t s = 0; s < c->sequence_len; s++) {
        const ComplexMatrix *gate = &c->gates[c->sequence[s]].matrix;

        pthread_t *threads = malloc(n_threads * sizeof(pthread_t));
        ThreadApplyTask *tasks = malloc(n_threads * sizeof(ThreadApplyTask));

        // Calcolo della ripartizione del lavoro tra i thread
        size_t block = dim / n_threads;
        size_t rem = dim % n_threads;
        size_t idx = 0;

        for (size_t t = 0; t < n_threads; t++) {
            size_t len = block + (rem > 0 ? 1 : 0);
            if (rem > 0) rem--;

            tasks[t].matrix = gate;
            tasks[t].input = current_in;
            tasks[t].output = current_out;
            tasks[t].start = idx;
            tasks[t].end = idx + len;

            // Creazione del thread per il calcolo della porzione assegnata
            pthread_create(&threads[t], NULL, thread_apply_matrix, &tasks[t]);
            idx += len;
        }

        // Sincronizzazione: attende che tutti i thread finiscano il gate corrente
        for (size_t t = 0; t < n_threads; t++)
            pthread_join(threads[t], NULL);

        // SWAP DEI DATI: Il risultato (output) diventa l'input per il gate successivo.
        // Si scambiano i puntatori agli array di dati per massimizzare l'efficienza.
        Complex *temp_data = current_in->data;
        current_in->data = current_out->data;
        current_out->data = temp_data;

        free(threads);
        free(tasks);
    }

    // Il vettore 'intermediate_state' ora contiene i dati vecchi, lo liberiamo.
    free_complex_vector(&intermediate_state);
}


/* LIBERAZIONE MEMORIA */

/**
 * Dealloca tutte le risorse dinamiche: gate, sequenze e vettore di stato.
 */
void circuit_free(Circuit *c) {
    for (size_t i = 0; i < c->gate_count; i++) {
        free(c->gates[i].name);
        free_complex_matrix(&c->gates[i].matrix);
    }
    free(c->gates);
    free(c->sequence);
    free_complex_vector(&c->state);
}


/* FUNZIONI DI OUTPUT/DEBUG */

void circuit_print_state(const Circuit *c) {
    print_complex_vector(&c->state);
}

void circuit_print_gate(const Circuit *c, size_t index) {
    if (index >= c->gate_count) return;
    printf("Gate %s:\n", c->gates[index].name);
    print_complex_matrix(&c->gates[index].matrix);
}

