/*
 * teste_casos.c -- SOLUCAO (Atividade 7, Passo 4)
 * Laboratorio de Gerencia de Memoria (Parte 1) -- slide 16
 *
 * Um teste para cada vizinhanca do processo X que termina:
 *   (a) processo | X | processo  -> X vira lacuna, sem fusao
 *   (b) processo | X | lacuna    -> funde com a direita
 *   (c) lacuna   | X | processo  -> funde com a esquerda
 *   (d) lacuna   | X | lacuna    -> funde com as duas
 *
 * Compilar:  gcc -Wall -Wextra -g teste_casos.c mem.c -o teste_casos   (ou: make)
 * Executar:  valgrind --leak-check=full ./teste_casos
 */
#include <stdio.h>
#include "mem.h"

enum { ESQ = 1, X = 2, DIR = 3, FIM = 4 };

/* Monta  ESQ(4) | X(4) | DIR(4) | FIM(4)  em 16 unidades e libera os
   vizinhos indicados antes de liberar X. */
static void caso(const char *nome, int libera_esq, int libera_dir) {
    memoria_t m;
    mem_inicia(&m, 16);
    mem_aloca(&m, ESQ, 4, FIRST_FIT);
    mem_aloca(&m, X,   4, FIRST_FIT);
    mem_aloca(&m, DIR, 4, FIRST_FIT);
    mem_aloca(&m, FIM, 4, FIRST_FIT);   /* impede fusao com o fim da memoria */
    if (libera_esq) mem_libera(&m, ESQ);
    if (libera_dir) mem_libera(&m, DIR);

    printf("caso %s\n  antes : ", nome);
    mem_imprime(&m);
    mem_libera(&m, X);
    printf("  depois: ");
    mem_imprime(&m);
    mem_destroi(&m);
}

int main(void) {
    caso("(a) processo | X | processo", 0, 0);
    caso("(b) processo | X | lacuna  ", 0, 1);
    caso("(c) lacuna   | X | processo", 1, 0);
    caso("(d) lacuna   | X | lacuna  ", 1, 1);
    return 0;
}
