/*
 * bitmap.h -- INTERFACE FORNECIDA (Atividade 6)
 * Laboratorio de Gerencia de Memoria (Parte 1) -- slides 14 e 15
 *
 * Gerenciador de memoria com mapa de bits: 1 bit por unidade de alocacao,
 * 1 = unidade ocupada, 0 = unidade livre.
 */
#ifndef BITMAP_H
#define BITMAP_H

#include <stdint.h>

typedef struct {
    uint8_t *bits;      /* 1 bit por unidade; 1 = ocupada */
    unsigned unidades;  /* quantidade de unidades         */
} bitmap_t;

void bm_inicia(bitmap_t *b, unsigned unidades);
void bm_marca(bitmap_t *b, unsigned inicio, unsigned tam, int ocupado);
int  bm_procura(const bitmap_t *b, unsigned k);   /* 1a sequencia de k zeros, ou -1 */
void bm_imprime(const bitmap_t *b);              /* 8 bits por linha, como no slide 14 */
void bm_para_lista(const bitmap_t *b);           /* P.0.5 -> L.5.3 -> ... (Passo 3) */
void bm_destroi(bitmap_t *b);

#endif
