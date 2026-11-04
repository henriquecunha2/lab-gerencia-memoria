/*
 * mem.c -- CODIGO BASE (Atividades 7 e 8)
 * Laboratorio de Gerencia de Memoria (Parte 1) -- slides 14 a 19
 *
 * Atividade 7: implemente FIRST_FIT, a divisao de lacunas em mem_aloca e
 *              a fusao de lacunas em mem_libera (quatro casos do slide 16).
 * Atividade 8: acrescente NEXT_FIT, BEST_FIT e WORST_FIT e some 1 a
 *              m->visitados a cada segmento examinado em qualquer busca.
 */
#include <stdio.h>
#include <stdlib.h>
#include "mem.h"

static segmento_t *novo(char tipo, int pid, unsigned inicio, unsigned tamanho) {
    segmento_t *s = malloc(sizeof *s);
    if (s == NULL) { perror("malloc"); exit(1); }
    s->tipo = tipo;
    s->pid = pid;
    s->inicio = inicio;
    s->tamanho = tamanho;
    s->ant = s->prox = NULL;
    return s;
}

void mem_inicia(memoria_t *m, unsigned total) {
    /* A memoria comeca como uma unica lacuna. */
    m->cabeca = novo('L', -1, 0, total);
    m->cursor = m->cabeca;
    m->total = total;
    m->visitados = 0;
}

int mem_aloca(memoria_t *m, int pid, unsigned tam, algoritmo_t alg) {
    /* TODO 1 (Ativ. 7): escolha a lacuna com FIRST_FIT.
       TODO 2 (Ativ. 7): se a lacuna for maior que tam, divida-a: o processo
                         fica no inicio e a sobra vira uma nova lacuna logo
                         depois dele.
       TODO 3 (Ativ. 8): NEXT_FIT, BEST_FIT e WORST_FIT; atualize m->cursor
                         e m->visitados conforme as regras do roteiro. */
    (void)m; (void)pid; (void)tam; (void)alg;
    return -1;
}

int mem_libera(memoria_t *m, int pid) {
    /* TODO 4 (Ativ. 7): ache o processo, transforme-o em lacuna e funda-o
                         com a lacuna vizinha da direita, da esquerda ou
                         com as duas. Se o segmento apontado por m->cursor
                         for fundido, o cursor passa a apontar para a
                         lacuna resultante. */
    (void)m; (void)pid;
    return -1;
}

void mem_imprime(const memoria_t *m) {
    for (const segmento_t *s = m->cabeca; s; s = s->prox)
        printf("%s%c·%u·%u", s == m->cabeca ? "" : " -> ", s->tipo, s->inicio, s->tamanho);
    printf("\n");
}

void mem_destroi(memoria_t *m) {
    segmento_t *s = m->cabeca;
    while (s) {
        segmento_t *prox = s->prox;
        free(s);
        s = prox;
    }
    m->cabeca = m->cursor = NULL;
    m->total = 0;
}
