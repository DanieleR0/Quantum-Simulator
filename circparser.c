#include "circparser.h"
#include "complex_matrix.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_LINE 4096
#define MAX_TOKENS 256

// Gestione centralizzata degli errori di parsing
static void parse_error(const char *msg) {
    fprintf(stderr, "Circ parser error: %s\n", msg);
    exit(EXIT_FAILURE);
}

// Rimuove spazi bianchi iniziali e finali per pulire i token
static char *trim(char *s) {
    while (isspace((unsigned char)*s)) s++;
    if (*s == 0) return s;
    char *end = s + strlen(s) - 1;
    while (end > s && isspace((unsigned char)*end)) {
        *end = 0;
        end--;
    }
    return s;
}

/**
 * Funzione interna per convertire stringhe in numeri complessi.
 * Gestisce formati standard (a+ib), compatti (i, -i) e a virgola (a, b).
 */
static Complex parse_complex(const char *s) {
    Complex z = {0.0, 0.0};
    char buf[128];
    strncpy(buf, s, 127);
    buf[127] = '\0';
    char *p = trim(buf);

    // Caso formato (reale, immaginario)
    char *comma = strchr(p, ',');
    if (comma) {
        *comma = '\0';
        char *re_str = trim(p);
        char *im_str = trim(comma + 1);
        z.real = strtod(re_str, NULL);
        
        if (strcmp(im_str, "i") == 0 || strcmp(im_str, "+i") == 0) z.imag = 1.0;
        else if (strcmp(im_str, "-i") == 0) z.imag = -1.0;
        else if (im_str[0] == 'i') z.imag = strtod(im_str + 1, NULL);
        else z.imag = strtod(im_str, NULL);
        return z;
    }

    double r = 0.0, i = 0.0;
    if (sscanf(p, "%lf+i%lf", &r, &i) == 2) { z.real = r; z.imag = i; }
    else if (sscanf(p, "%lf-i%lf", &r, &i) == 2) { z.real = r; z.imag = -i; }
    else if (strcmp(p, "i") == 0 || strcmp(p, "+i") == 0) { z.imag = 1.0; }
    else if (strcmp(p, "-i") == 0) { z.imag = -1.0; }
    else { z.real = strtod(p, NULL); }

    return z;
}

void parse_circ_file(const char *filename, Circuit *c) {
    FILE *fp = fopen(filename, "r");
    if (!fp) parse_error("Cannot open circuit file");

    char line[MAX_LINE];
    while (fgets(line, sizeof(line), fp)) {
        char *l = trim(line);
        if (*l == '\0') continue;

        // Definizione del numero di qubit
        if (strncmp(l, "#qubits", 7) == 0) {
            unsigned int n;
            if (sscanf(l, "#qubits %u", &n) != 1) parse_error("Invalid #qubits");
            circuit_init(c, n);
        } 
        // Definizione di un nuovo operatore (Gate)
        else if (strncmp(l, "#define", 7) == 0) {
            char name[32];
            if (sscanf(l, "#define %31s", name) != 1) parse_error("Invalid gate name");

            size_t dim = 1UL << c->n_qubits;
            size_t buffer_size = dim * dim * 64 + MAX_LINE;
            char *buffer = malloc(buffer_size); // Allocazione dinamica per sistemi grandi 
            
            char *start = strchr(l, '[');
            if (!start) parse_error("Invalid matrix: missing '['");
            strcpy(buffer, start);

            // Accumula le righe della matrice finché non trova la chiusura ']'
            while (!strchr(buffer, ']')) {
                if (!fgets(line, sizeof(line), fp)) {
                    free(buffer);
                    parse_error("Missing closing ] in matrix");
                }
                strcat(buffer, line);
            }

            // Pulizia delle parentesi tonde dalla stringa della matrice
            char *mat_start = strchr(buffer, '[');
            char *mat_end = strchr(buffer, ']');
            size_t len = mat_end - mat_start - 1;
            char *clean = malloc(len + 1);
            size_t j = 0;
            for (size_t i = 0; i < len; i++) {
                if (mat_start[1+i] != '(' && mat_start[1+i] != ')')
                    clean[j++] = mat_start[1+i];
            }
            clean[j] = 0;

            // Parsing dei singoli elementi complessi della matrice
            ComplexMatrix mat = alloc_complex_matrix(dim, dim);
            char *tok = strtok(clean, " \t\n\r");
            size_t idx = 0;
            while (tok && idx < dim * dim) {
                mat.data[idx++] = parse_complex(tok);
                tok = strtok(NULL, " \t\n\r");
            }

            circuit_add_gate(c, name, mat);
            free(buffer); free(clean);
        }
        // Definizione della sequenza di esecuzione del circuito
        else if (strncmp(l, "#circ", 5) == 0) {
            char *p = l + 5;
            char *tokens[MAX_TOKENS];
            int count = 0;
            char *tok = strtok(p, " \t\n\r");
            while (tok && count < MAX_TOKENS) {
                tokens[count++] = tok;
                tok = strtok(NULL, " \t\n\r");
            }
            // Mappatura dei nomi dei gate ai loro indici interni
            size_t *seq = malloc(sizeof(size_t) * count);
            for (int i = 0; i < count; i++) {
                size_t k;
                for (k = 0; k < c->gate_count; k++) {
                    if (strcmp(c->gates[k].name, tokens[i]) == 0) break;
                }
                if (k == c->gate_count) parse_error("Gate not defined");
                seq[i] = k;
            }
            circuit_set_sequence(c, seq, count);
            free(seq);
        }
    }
    fclose(fp);
}