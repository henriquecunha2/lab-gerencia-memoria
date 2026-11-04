/*
 * slide19.c -- SOLUCAO (Atividade 8, Passo 3)
 * Laboratorio de Gerencia de Memoria (Parte 1) -- slide 19
 *
 * Lacunas, em ordem de endereco: 10, 4, 20, 18, 7, 9, 12 e 15, cada uma
 * seguida de um separador de 5 unidades (135 unidades no total). Para cada
 * algoritmo, sobre uma copia nova da memoria, atende 12, 10 e 9 e informa
 * o tamanho da lacuna que cada pedido tomou.
 *
 * Compilar:  gcc -Wall -Wextra -g slide19.c mem.c -o slide19   (ou: make)
 */
#include <stdio.h>
#include "mem.h"

static const unsigned LACUNAS[] = { 10, 4, 20, 18, 7, 9, 12, 15 };
#define NLAC (int)(sizeof LACUNAS / sizeof LACUNAS[0])
static const unsigned PEDIDOS[] = { 12, 10, 9 };
#define NPED (int)(sizeof PEDIDOS / sizeof PEDIDOS[0])

static void monta(memoria_t *m) {
    int pid = 1;
    mem_inicia(m, 135);
    for (int i = 0; i < NLAC; i++) {
        mem_aloca(m, 100 + i, LACUNAS[i], FIRST_FIT);  /* temporario */
        mem_aloca(m, pid++, 5, FIRST_FIT);             /* separador  */
    }
    for (int i = 0; i < NLAC; i++) mem_libera(m, 100 + i);
    m->cursor = m->cabeca;
    m->visitados = 0;
}

/* Tamanho da lacuna que comeca em 'inicio' (0 se nao houver). */
static unsigned lacuna_em(const memoria_t *m, unsigned inicio) {
    for (const segmento_t *s = m->cabeca; s; s = s->prox)
        if (s->tipo == 'L' && s->inicio == inicio) return s->tamanho;
    return 0;
}

int main(void) {
    const char *nome[] = { "First fit", "Next fit", "Best fit", "Worst fit" };
    memoria_t m;

    monta(&m);
    printf("memoria inicial: ");
    mem_imprime(&m);
    mem_destroi(&m);

    printf("\nAlgoritmo   12 KB  10 KB   9 KB   (lacuna tomada por cada pedido)\n");
    for (int a = FIRST_FIT; a <= WORST_FIT; a++) {
        monta(&m);
        printf("%-10s", nome[a]);
        for (int i = 0; i < NPED; i++) {
            int ini = mem_aloca(&m, 200 + i, PEDIDOS[i], (algoritmo_t)a);
            if (ini < 0) { printf("  falhou"); continue; }
            /* lacuna tomada = pedido + sobra que ficou logo depois dele */
            printf("  %5u", PEDIDOS[i] + lacuna_em(&m, (unsigned)ini + PEDIDOS[i]));
        }
        printf("\n");
        mem_destroi(&m);
    }
    return 0;
}
