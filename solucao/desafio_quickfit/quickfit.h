/*
 * quickfit.h -- SOLUCAO (Desafio -- quick fit)
 * Laboratorio de Gerencia de Memoria (Parte 1) -- slide 18
 *
 * Uma lista de lacunas para cada tamanho exato (4, 8, 16, 32 e 64
 * unidades) e uma lista geral, ordenada por endereco, para os demais.
 */
#ifndef QUICKFIT_H
#define QUICKFIT_H

#define QF_NEXATAS 5

typedef struct lacuna {
    unsigned inicio, tamanho;
    struct lacuna *prox;
} lacuna_t;

typedef struct {
    lacuna_t *exata[QF_NEXATAS];    /* tamanhos 4, 8, 16, 32 e 64 (pilhas) */
    lacuna_t *geral;                /* demais tamanhos, por endereco       */
    unsigned total;
    unsigned long visitados;        /* lacunas examinadas nas buscas       */
    unsigned intervalo_fusao;       /* 0 = nunca funde                     */
    unsigned long liberacoes;
    unsigned long fusoes;           /* quantas vezes a fusao rodou         */
    unsigned long examinados_fusao; /* lacunas examinadas pelas fusoes     */
} quickfit_t;

void qf_inicia(quickfit_t *q, unsigned total, unsigned intervalo_fusao);
int  qf_aloca(quickfit_t *q, unsigned tam);                 /* inicio ou -1 */
void qf_libera(quickfit_t *q, unsigned inicio, unsigned tam);
void qf_funde(quickfit_t *q);
void qf_mede(const quickfit_t *q, unsigned *livre, unsigned *maior, unsigned *nlacunas);
void qf_destroi(quickfit_t *q);

#endif
