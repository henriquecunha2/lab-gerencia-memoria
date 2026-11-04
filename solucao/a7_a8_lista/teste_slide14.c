/*
 * teste_slide14.c -- Atividade 7, Passo 3
 * Laboratorio de Gerencia de Memoria (Parte 1) -- slides 14 e 16
 *
 * Monta a memoria do slide 14 (32 unidades) com FIRST_FIT e libera C
 * (caso (b) do slide 16). Saidas esperadas:
 *   P·0·5 -> L·5·3 -> P·8·6 -> P·14·4 -> L·18·2 -> P·20·6 -> P·26·3 -> L·29·3
 *   P·0·5 -> L·5·3 -> P·8·6 -> L·14·6 -> P·20·6 -> P·26·3 -> L·29·3
 *
 * Compilar:  gcc -Wall -Wextra -g teste_slide14.c mem.c -o teste_slide14   (ou: make)
 */
#include <stdio.h>
#include "mem.h"

enum { A = 1, X1, B, C, X2, D, E };

int main(void) {
    memoria_t m;
    mem_inicia(&m, 32);
    mem_aloca(&m, A,  5, FIRST_FIT);
    mem_aloca(&m, X1, 3, FIRST_FIT);
    mem_aloca(&m, B,  6, FIRST_FIT);
    mem_aloca(&m, C,  4, FIRST_FIT);
    mem_aloca(&m, X2, 2, FIRST_FIT);
    mem_aloca(&m, D,  6, FIRST_FIT);
    mem_aloca(&m, E,  3, FIRST_FIT);
    mem_libera(&m, X1);
    mem_libera(&m, X2);
    printf("slide 14      : ");
    mem_imprime(&m);

    mem_libera(&m, C);
    printf("libera C (b)  : ");
    mem_imprime(&m);

    mem_destroi(&m);
    return 0;
}
