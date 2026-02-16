#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "initparser.h"
#include "circparser.h"

/**
 * Punto di ingresso del simulatore.
 * Gestisce gli argomenti da riga di comando tramite getopt.
 */
int main(int argc, char *argv[]) {
    char *init_file = NULL;
    char *circ_file = NULL;
    int n_threads = 1;

    int opt;
    // Parsing delle opzioni: -i (input init), -c (input circuito), -t (threads)
    while ((opt = getopt(argc, argv, "i:c:t:")) != -1) {
        switch (opt) {
            case 'i': init_file = optarg; break;
            case 'c': circ_file = optarg; break;
            case 't': n_threads = atoi(optarg); break;
            default:
                fprintf(stderr, "Uso: %s -i init.q -c circ.q [-t threads]\n", argv[0]);
                return EXIT_FAILURE;
        }
    }

    // Verifica che i file obbligatori siano stati forniti
    if (!init_file || !circ_file) {
        fprintf(stderr, "Errore: File di inizializzazione e circuito richiesti.\n");
        fprintf(stderr, "Uso: %s -i init.q -c circ.q [-t threads]\n", argv[0]);
        return EXIT_FAILURE;
    }

    Circuit circuit;
    
    // Caricamento dei dati dai file
    parse_init_file(init_file, &circuit);
    parse_circ_file(circ_file, &circuit);

    // Esecuzione della simulazione parallela
    circuit_execute_parallel(&circuit, n_threads);

    // Output del risultato finale
    circuit_print_state(&circuit);

    // Pulizia della memoria
    circuit_free(&circuit);

    return EXIT_SUCCESS;
}