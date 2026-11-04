/*
 * teste_slide14.c -- Atividade 6, Passos 2 e 3
 * Laboratorio de Gerencia de Memoria (Parte 1) -- slide 14
 *
 * Monta a memoria do slide 14 (32 unidades) no mapa de bits, imprime o
 * mapa e a lista que se obtem a partir dele.
 *
 * Compilar:  gcc -Wall -Wextra -g teste_slide14.c bitmap.c -o teste_slide14   (ou: make)
 */
#include <stdio.h>
#include "bitmap.h"

int main(void) {
    bitmap_t b;
    bm_inicia(&b, 32);
    bm_marca(&b,  0, 5, 1);     /* A */
    bm_marca(&b,  8, 6, 1);     /* B */
    bm_marca(&b, 14, 4, 1);     /* C */
    bm_marca(&b, 20, 6, 1);     /* D */
    bm_marca(&b, 26, 3, 1);     /* E */

    printf("Mapa de bits (Passo 2):\n");
    bm_imprime(&b);
    printf("\nLista obtida do mapa (Passo 3):\n");
    bm_para_lista(&b);
    printf("\nPrimeira sequencia livre de 3 unidades: %d\n", bm_procura(&b, 3));

    bm_destroi(&b);
    return 0;
}
