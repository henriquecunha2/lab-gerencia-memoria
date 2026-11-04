/*
 * bitmap.c -- CODIGO BASE (Atividade 6)
 * Laboratorio de Gerencia de Memoria (Parte 1) -- slides 14 e 15
 *
 * Implemente as funcoes de bitmap.h usando operacoes de bits (>>, &, |):
 * cada byte de b->bits guarda 8 unidades. Nao use um char por unidade.
 * Sugestao: a unidade i fica no byte i >> 3, no bit 7 - (i & 7), assim
 * a impressao byte a byte sai na mesma ordem do slide 14.
 */
#include <stdio.h>
#include <stdlib.h>
#include "bitmap.h"

void bm_inicia(bitmap_t *b, unsigned unidades) {
    /* TODO 1: aloque (unidades + 7) / 8 bytes zerados (calloc) */
    b->bits = NULL;
    b->unidades = unidades;
}

void bm_marca(bitmap_t *b, unsigned inicio, unsigned tam, int ocupado) {
    /* TODO 2: ligue (ocupado != 0) ou desligue os bits [inicio, inicio + tam) */
    (void)b; (void)inicio; (void)tam; (void)ocupado;
}

int bm_procura(const bitmap_t *b, unsigned k) {
    /* TODO 3: devolva o inicio da primeira sequencia de k bits 0, ou -1 */
    (void)b; (void)k;
    return -1;
}

void bm_imprime(const bitmap_t *b) {
    /* TODO 4: imprima 8 bits por linha, como no slide 14 */
    (void)b;
}

void bm_para_lista(const bitmap_t *b) {
    /* TODO 5 (Passo 3): percorra o mapa e imprima as sequencias de 1s como
       P e as de 0s como L, no formato  P·0·5 -> L·5·3 -> ...  */
    (void)b;
}

void bm_destroi(bitmap_t *b) {
    free(b->bits);
    b->bits = NULL;
    b->unidades = 0;
}
