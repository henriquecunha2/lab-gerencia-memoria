/*
 * bitmap.c -- SOLUCAO (Atividade 6)
 * Laboratorio de Gerencia de Memoria (Parte 1) -- slides 14 e 15
 *
 * A unidade i fica no byte i >> 3, no bit 7 - (i & 7): a impressao byte a
 * byte sai na mesma ordem do slide 14.
 */
#include <stdio.h>
#include <stdlib.h>
#include "bitmap.h"

#define MASCARA(i) ((uint8_t)(0x80u >> ((i) & 7u)))

static int bm_bit(const bitmap_t *b, unsigned i) {
    return (b->bits[i >> 3] & MASCARA(i)) != 0;
}

void bm_inicia(bitmap_t *b, unsigned unidades) {
    b->bits = calloc((unidades + 7) / 8, 1);
    if (b->bits == NULL) { perror("calloc"); exit(1); }
    b->unidades = unidades;
}

void bm_marca(bitmap_t *b, unsigned inicio, unsigned tam, int ocupado) {
    for (unsigned i = inicio; i < inicio + tam && i < b->unidades; i++) {
        if (ocupado) b->bits[i >> 3] |= MASCARA(i);
        else         b->bits[i >> 3] &= (uint8_t)~MASCARA(i);
    }
}

int bm_procura(const bitmap_t *b, unsigned k) {
    unsigned corrida = 0;               /* zeros consecutivos ate aqui */
    if (k == 0) return 0;
    for (unsigned i = 0; i < b->unidades; i++) {
        if (bm_bit(b, i)) corrida = 0;
        else if (++corrida == k) return (int)(i - k + 1);
    }
    return -1;
}

void bm_imprime(const bitmap_t *b) {
    for (unsigned i = 0; i < b->unidades; i++) {
        putchar(bm_bit(b, i) ? '1' : '0');
        if ((i & 7) == 7 || i + 1 == b->unidades) putchar('\n');
    }
}

void bm_para_lista(const bitmap_t *b) {
    unsigned i = 0;
    while (i < b->unidades) {
        int v = bm_bit(b, i);
        unsigned inicio = i;
        while (i < b->unidades && bm_bit(b, i) == v) i++;
        printf("%s%c·%u·%u", inicio ? " -> " : "", v ? 'P' : 'L', inicio, i - inicio);
    }
    printf("\n");
}

void bm_destroi(bitmap_t *b) {
    free(b->bits);
    b->bits = NULL;
    b->unidades = 0;
}
