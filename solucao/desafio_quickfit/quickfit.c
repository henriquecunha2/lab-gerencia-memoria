/*
 * quickfit.c -- SOLUCAO (Desafio -- quick fit)
 * Laboratorio de Gerencia de Memoria (Parte 1) -- slide 18
 *
 * Regras:
 *  - pedido de tamanho exato (4, 8, 16, 32, 64): retira a primeira lacuna
 *    da lista correspondente; se ela estiver vazia, usa first fit na geral;
 *  - demais pedidos: first fit na lista geral;
 *  - a sobra de uma lacuna dividida vai para a lista do seu tamanho;
 *  - ao liberar, a lacuna vai para a lista do seu tamanho, SEM fusao com
 *    as vizinhas;
 *  - se intervalo_fusao > 0, a cada intervalo_fusao liberacoes qf_funde
 *    junta todas as lacunas, ordena por endereco, funde as adjacentes e
 *    redistribui o resultado entre as listas.
 */
#include <stdio.h>
#include <stdlib.h>
#include "quickfit.h"

static const unsigned EXATOS[QF_NEXATAS] = { 4, 8, 16, 32, 64 };

static int indice_exato(unsigned tam) {
    for (int i = 0; i < QF_NEXATAS; i++)
        if (EXATOS[i] == tam) return i;
    return -1;
}

static lacuna_t *nova(unsigned inicio, unsigned tamanho) {
    lacuna_t *l = malloc(sizeof *l);
    if (l == NULL) { perror("malloc"); exit(1); }
    l->inicio = inicio;
    l->tamanho = tamanho;
    l->prox = NULL;
    return l;
}

/* Coloca a lacuna na lista do seu tamanho (custo nao contado). */
static void guarda(quickfit_t *q, lacuna_t *l) {
    int i = indice_exato(l->tamanho);
    if (i >= 0) {                   /* lista exata: empilha */
        l->prox = q->exata[i];
        q->exata[i] = l;
    } else {                        /* lista geral: ordenada por endereco */
        lacuna_t **pp = &q->geral;
        while (*pp && (*pp)->inicio < l->inicio) pp = &(*pp)->prox;
        l->prox = *pp;
        *pp = l;
    }
}

void qf_inicia(quickfit_t *q, unsigned total, unsigned intervalo_fusao) {
    for (int i = 0; i < QF_NEXATAS; i++) q->exata[i] = NULL;
    q->geral = NULL;
    q->total = total;
    q->visitados = 0;
    q->intervalo_fusao = intervalo_fusao;
    q->liberacoes = q->fusoes = q->examinados_fusao = 0;
    guarda(q, nova(0, total));
}

int qf_aloca(quickfit_t *q, unsigned tam) {
    int i = indice_exato(tam);
    lacuna_t **pp, *l;
    unsigned inicio;

    if (i >= 0 && q->exata[i]) {    /* acerto exato: O(1) */
        l = q->exata[i];
        q->visitados++;
        q->exata[i] = l->prox;
        inicio = l->inicio;
        free(l);
        return (int)inicio;
    }

    for (pp = &q->geral; *pp; pp = &(*pp)->prox) {     /* first fit na geral */
        q->visitados++;
        if ((*pp)->tamanho >= tam) break;
    }
    if (*pp == NULL) return -1;

    l = *pp;
    *pp = l->prox;
    inicio = l->inicio;
    if (l->tamanho > tam) {         /* a sobra vai para a lista do seu tamanho */
        l->inicio += tam;
        l->tamanho -= tam;
        guarda(q, l);
    } else {
        free(l);
    }
    return (int)inicio;
}

void qf_libera(quickfit_t *q, unsigned inicio, unsigned tam) {
    guarda(q, nova(inicio, tam));
    q->liberacoes++;
    if (q->intervalo_fusao && q->liberacoes % q->intervalo_fusao == 0)
        qf_funde(q);
}

static int por_endereco(const void *a, const void *b) {
    const lacuna_t *x = *(lacuna_t *const *)a, *y = *(lacuna_t *const *)b;
    return (x->inicio > y->inicio) - (x->inicio < y->inicio);
}

void qf_funde(quickfit_t *q) {
    lacuna_t *todas[4096];
    int n = 0;

    /* junta as lacunas de todas as listas */
    for (int i = 0; i <= QF_NEXATAS; i++) {
        lacuna_t **cab = i < QF_NEXATAS ? &q->exata[i] : &q->geral;
        for (lacuna_t *l = *cab; l; l = l->prox) todas[n++] = l;
        *cab = NULL;
    }
    q->examinados_fusao += n;
    q->fusoes++;

    qsort(todas, n, sizeof todas[0], por_endereco);

    /* funde as adjacentes e redistribui */
    for (int i = 0; i < n; ) {
        lacuna_t *l = todas[i++];
        while (i < n && l->inicio + l->tamanho == todas[i]->inicio) {
            l->tamanho += todas[i]->tamanho;
            free(todas[i++]);
        }
        guarda(q, l);
    }
}

void qf_mede(const quickfit_t *q, unsigned *livre, unsigned *maior, unsigned *nlacunas) {
    *livre = *maior = *nlacunas = 0;
    for (int i = 0; i <= QF_NEXATAS; i++)
        for (const lacuna_t *l = i < QF_NEXATAS ? q->exata[i] : q->geral; l; l = l->prox) {
            *livre += l->tamanho;
            if (l->tamanho > *maior) *maior = l->tamanho;
            (*nlacunas)++;
        }
}

void qf_destroi(quickfit_t *q) {
    for (int i = 0; i <= QF_NEXATAS; i++) {
        lacuna_t *l = i < QF_NEXATAS ? q->exata[i] : q->geral;
        while (l) { lacuna_t *p = l->prox; free(l); l = p; }
    }
    for (int i = 0; i < QF_NEXATAS; i++) q->exata[i] = NULL;
    q->geral = NULL;
}
