#ifndef INITPARSER_H
#define INITPARSER_H

#include "circuit.h"

/**
 * Legge il file di inizializzazione per configurare il sistema.
 * Gestisce le direttive #qubits e #init.
 * Input: filename, c (puntatore alla struttura Circuit)
 */
void parse_init_file(const char *filename, Circuit *c);

#endif