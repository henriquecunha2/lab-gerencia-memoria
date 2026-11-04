/*
 * mem.h -- INTERFACE FORNECIDA (Atividades 7 e 8)
 * Laboratorio de Gerencia de Memoria (Parte 1) -- slides 14 a 19
 *
 * Gerenciador de memoria com lista duplamente encadeada de segmentos
 * (processos e lacunas), ordenada por endereco. Nao altere esta interface.
 */
#ifndef MEM_H
#define MEM_H

typedef enum { FIRST_FIT, NEXT_FIT, BEST_FIT, WORST_FIT } algoritmo_t;

typedef struct segmento {
    char tipo;                 /* 'P' = processo, 'L' = lacuna */
    int pid;                   /* -1 nas lacunas */
    unsigned inicio, tamanho;  /* em unidades de alocacao */
    struct segmento *ant, *prox;
} segmento_t;

typedef struct {
    segmento_t *cabeca;        /* lista ordenada por endereco */
    segmento_t *cursor;        /* onde o next fit parou */
    unsigned total;            /* tamanho da memoria */
    unsigned long visitados;   /* segmentos examinados nas buscas */
} memoria_t;

void mem_inicia(memoria_t *m, unsigned total);
int  mem_aloca(memoria_t *m, int pid, unsigned tam, algoritmo_t alg); /* inicio ou -1 */
int  mem_libera(memoria_t *m, int pid);                                /* 0 ou -1 */
void mem_imprime(const memoria_t *m);                                  /* P·0·5 -> L·5·3 ... */
void mem_destroi(memoria_t *m);

#endif
