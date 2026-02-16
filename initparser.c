#include "initparser.h"
#include "complex_vector.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_LINE 4096 

static void parse_error(const char *msg) {
    fprintf(stderr, "Init parser error: %s\n", msg);
    exit(EXIT_FAILURE);
}

// Rimuove spazi bianchi all'inizio e alla fine della stringa
static char *trim(char *s) {
    while (isspace((unsigned char)*s)) s++;
    if (*s == 0) return s;
    char *end = s + strlen(s) - 1;
    while (end > s && isspace((unsigned char)*end)) *end-- = 0;
    return s;
}

// Converte una stringa in un numero complesso (supporta a+ib, a-ib, i, -i) - aggiornata
static Complex parse_complex(const char *s) {
    Complex z = {0.0, 0.0};
    char *p = (char*)s;

    // Gestione casi speciali per la sola parte immaginaria "i" o "-i"
    if (strcmp(s, "i") == 0 || strcmp(s, "+i") == 0) { z.imag = 1.0; return z; }
    if (strcmp(s, "-i") == 0) { z.imag = -1.0; return z; }

    // Prova a leggere la parte reale
    z.real = strtod(p, &p);

    // Salta eventuali spazi tra numero e segno
    while(isspace((unsigned char)*p)) p++;

    if (*p == '+') {
        p++;
        while(isspace((unsigned char)*p)) p++;
        if (*p == 'i') z.imag = strtod(p + 1, NULL); // Caso +i5
    }
    else if (*p == '-') {
        p++;
        while(isspace((unsigned char)*p)) p++;
        if (*p == 'i') z.imag = -strtod(p + 1, NULL); // Caso -i5
    }
    return z;
}

void parse_init_file(const char *filename, Circuit *c) {
    FILE *fp = fopen(filename, "r");
    if (!fp) parse_error("Cannot open init file");

    char line[MAX_LINE];
    int qubits_set = 0;
    int init_set   = 0;

    while (fgets(line, sizeof(line), fp)) {
        char *l = trim(line);

        // Ignora righe vuote e commenti che iniziano con %
        if (*l == '\0' || *l == '%') continue;

        /* Direttiva #qubits: definisce la dimensione del sistema */
        if (strncmp(l, "#qubits", 7) == 0) {
            unsigned int n;
            if (sscanf(l, "#qubits %u", &n) != 1) parse_error("Invalid #qubits");
            circuit_init(c, n);
            qubits_set = 1;
        }
        /* Direttiva #init: legge il vettore di stato iniziale */
        else if (strncmp(l, "#init", 5) == 0) {
            if (!qubits_set) parse_error("#init before #qubits");

            size_t dim = 1UL << c->n_qubits;
            // Allocazione dinamica del buffer di lettura per supportare H10 - test maledetto
            size_t buffer_size = dim * 64 + MAX_LINE; 
            char *buffer = malloc(buffer_size);
            if (!buffer) parse_error("Memory allocation failed");
            buffer[0] = '\0';

            // Individua l'inizio del vettore
            char *start = strchr(l, '[');
            if (!start) parse_error("Invalid #init syntax: missing '['");
            strcpy(buffer, start);

            // Legge più righe finché non trova la chiusura ']'
            while (!strchr(buffer, ']')) {
                if (!fgets(line, sizeof(line), fp)) {
                    free(buffer);
                    parse_error("Missing closing ] in #init");
                }
                strcat(buffer, line);
            }

            char *p_open = strchr(buffer, '[');
            char *p_close = strchr(buffer, ']');
            size_t len = p_close - p_open - 1;

            char *content = malloc(len + 1);
            strncpy(content, p_open + 1, len);
            content[len] = '\0';

            // Tokenizzazione dei valori separati da virgola
            char *tok = strtok(content, ",");
            size_t idx = 0;

            while (tok && idx < dim) {
                c->state.data[idx++] = parse_complex(trim(tok));
                tok = strtok(NULL, ",");
            }

            if (idx != dim) parse_error("Wrong number of init amplitudes");

            free(buffer);
            free(content);
            init_set = 1;
        }
        else {
            parse_error("Unknown directive in init file");
        }
    }
    fclose(fp);
}