/*
 * mem.c -- SOLUCAO (Atividades 7 e 8)
 * Laboratorio de Gerencia de Memoria (Parte 1) -- slides 14 a 19
 *
 * Lista duplamente encadeada de segmentos, ordenada por endereco, com os
 * algoritmos first fit, next fit, best fit e worst fit e fusao de lacunas
 * na liberacao (os quatro casos do slide 16).
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

/* Retira s da lista (sem liberar). */
static void desliga(memoria_t *m, segmento_t *s) {
    if (s->ant) s->ant->prox = s->prox; else m->cabeca = s->prox;
    if (s->prox) s->prox->ant = s->ant;
}

void mem_inicia(memoria_t *m, unsigned total) {
    m->cabeca = novo('L', -1, 0, total);
    m->cursor = m->cabeca;
    m->total = total;
    m->visitados = 0;
}

static int serve(const segmento_t *s, unsigned tam) {
    return s->tipo == 'L' && s->tamanho >= tam;
}

/* Escolhe a lacuna segundo o algoritmo, contando os segmentos examinados. */
static segmento_t *escolhe(memoria_t *m, unsigned tam, algoritmo_t alg) {
    segmento_t *s, *melhor = NULL;

    switch (alg) {
    case FIRST_FIT:
        for (s = m->cabeca; s; s = s->prox) {
            m->visitados++;
            if (serve(s, tam)) return s;
        }
        return NULL;

    case NEXT_FIT: {
        segmento_t *partida = m->cursor ? m->cursor : m->cabeca;
        s = partida;
        do {
            m->visitados++;
            if (serve(s, tam)) return s;
            s = s->prox ? s->prox : m->cabeca;      /* da a volta */
        } while (s != partida);
        return NULL;
    }

    case BEST_FIT:
    case WORST_FIT:
        for (s = m->cabeca; s; s = s->prox) {
            m->visitados++;
            if (!serve(s, tam)) continue;
            if (melhor == NULL
                || (alg == BEST_FIT  && s->tamanho < melhor->tamanho)
                || (alg == WORST_FIT && s->tamanho > melhor->tamanho))
                melhor = s;
        }
        return melhor;
    }
    return NULL;
}

int mem_aloca(memoria_t *m, int pid, unsigned tam, algoritmo_t alg) {
    segmento_t *s;
    if (tam == 0) return -1;
    s = escolhe(m, tam, alg);
    if (s == NULL) return -1;

    if (s->tamanho > tam) {         /* divide: a sobra vira lacuna logo depois */
        segmento_t *sobra = novo('L', -1, s->inicio + tam, s->tamanho - tam);
        sobra->ant = s;
        sobra->prox = s->prox;
        if (s->prox) s->prox->ant = sobra;
        s->prox = sobra;
        s->tamanho = tam;
    }
    s->tipo = 'P';
    s->pid = pid;
    m->cursor = s;                  /* o next fit continua daqui */
    return (int)s->inicio;
}

int mem_libera(memoria_t *m, int pid) {
    segmento_t *s, *viz;

    for (s = m->cabeca; s; s = s->prox)
        if (s->tipo == 'P' && s->pid == pid) break;
    if (s == NULL) return -1;

    s->tipo = 'L';
    s->pid = -1;

    viz = s->prox;                  /* funde com a lacuna da direita */
    if (viz && viz->tipo == 'L') {
        s->tamanho += viz->tamanho;
        desliga(m, viz);
        if (m->cursor == viz) m->cursor = s;
        free(viz);
    }
    viz = s->ant;                   /* funde com a lacuna da esquerda */
    if (viz && viz->tipo == 'L') {
        viz->tamanho += s->tamanho;
        desliga(m, s);
        if (m->cursor == s) m->cursor = viz;
        free(s);
    }
    return 0;
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
