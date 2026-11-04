/*
 * bench.c -- CODIGO BASE (Atividade 6, Passo 4)
 * Laboratorio de Gerencia de Memoria (Parte 1) -- slide 15
 *
 * Mede o tempo medio de bm_procura em uma memoria de 64 MiB com 90% das
 * unidades ocupadas em posicoes sorteadas, para k = 8, 64 e 512 e unidades
 * de 4 B, 64 B e 4 KiB.
 *
 * Compilar:  gcc -Wall -Wextra -O2 bench.c bitmap.c -o bench   (ou: make)
 */
#define _POSIX_C_SOURCE 199309L
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "bitmap.h"

#define MEMORIA     (64u * 1024u * 1024u)   /* 64 MiB */
#define REPETICOES  100

static double agora(void) {
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return ts.tv_sec + ts.tv_nsec / 1e9;
}

int main(void) {
    const unsigned unidade[] = { 4, 64, 4096 };
    const unsigned ks[] = { 8, 64, 512 };

    printf("unidade  unidades     k   resultado   tempo medio (ms)\n");
    for (int u = 0; u < 3; u++) {
        bitmap_t b;
        unsigned n = MEMORIA / unidade[u];
        bm_inicia(&b, n);
        srand(1);
        /* TODO 1: marque cada unidade como ocupada com probabilidade 0,9 */

        for (int j = 0; j < 3; j++) {
            int r = -1;
            double t0 = agora();
            /* TODO 2: repita a busca de ks[j] zeros REPETICOES vezes */
            double ms = (agora() - t0) * 1000.0 / REPETICOES;
            printf("%5u B  %9u  %4u  %10s  %10.3f\n", unidade[u], n, ks[j],
                   r < 0 ? "falhou" : "achou", ms);
        }
        bm_destroi(&b);
    }
    return 0;
}
