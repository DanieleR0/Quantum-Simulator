#ifndef CIRCPARSER_H
#define CIRCPARSER_H

#include "circuit.h"

/**
 * Parser per il file del circuito quantistico.
 * Legge le definizioni dei gate (#define) e la sequenza operativa (#circ).
 * Input: filename, c (puntatore alla struttura Circuit)
 */
void parse_circ_file(const char *filename, Circuit *c);

#endif