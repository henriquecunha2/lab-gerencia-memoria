/*
 * troca.c -- SOLUCAO (Atividade 7, Passo 5)
 * Laboratorio de Gerencia de Memoria (Parte 1) -- slide 11
 *
 * Reproduz a sequencia da Atividade 5 (em K): memoria de 1024K, SO de 0K
 * a 128K (processo 0) e first fit.
 *
 * Compilar:  gcc -Wall -Wextra -g troca.c mem.c -o troca   (ou: make)
 */
#include <stdio.h>
#include "mem.h"

enum { SO = 0, A, B, C, D };

static void quadro(const char *q, const char *evento, const memoria_t *m) {
    printf("%s %-20s ", q, evento);
    mem_imprime(m);
}

int main(void) {
    memoria_t m;
    mem_inicia(&m, 1024);
    mem_aloca(&m, SO, 128, FIRST_FIT);

    mem_aloca(&m, A, 192, FIRST_FIT); quadro("(a)", "A entra", &m);
    mem_aloca(&m, B, 128, FIRST_FIT); quadro("(b)", "B entra", &m);
    mem_aloca(&m, C, 192, FIRST_FIT); quadro("(c)", "C entra", &m);
    mem_libera(&m, A);                quadro("(d)", "A e levado ao disco", &m);
    mem_aloca(&m, D, 128, FIRST_FIT); quadro("(e)", "D entra", &m);
    mem_libera(&m, B);                quadro("(f)", "B termina", &m);
    mem_aloca(&m, A, 192, FIRST_FIT); quadro("(g)", "A volta do disco", &m);

    mem_destroi(&m);
    return 0;
}
