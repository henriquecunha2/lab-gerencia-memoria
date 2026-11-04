/*
 * experimento.c -- SOLUCAO (Atividade 8, Passo 4)
 * Laboratorio de Gerencia de Memoria (Parte 1) -- slides 17 e 18
 *
 * Carga igual para os quatro algoritmos:
 *   memoria de 1024 unidades, inicialmente vazia; 20 000 operacoes, das
 *   quais as 2 000 primeiras sao aquecimento; 55% alocacao e 45% liberacao
 *   de um processo vivo sorteado (sempre alocacao se nao houver processo
 *   vivo); tamanho uniforme entre 1 e 64; sementes 1 a 5.
 *
 * Metricas (medias fora do aquecimento):
 *   falhas            pedidos de alocacao que falharam / pedidos
 *   frag. externa     1 - (maior lacuna / total livre), apos cada operacao
 *                     (0 quando nao ha memoria livre)
 *   lacunas           numero de lacunas, apos cada operacao
 *   lacunas 1-2       lacunas de 1 ou 2 unidades, apos cada operacao (Q8.3)
 *   visitados/pedido  segmentos examinados por pedido de alocacao
 *
 * Compilar:  gcc -Wall -Wextra -O2 experimento.c mem.c -o experimento   (ou: make)
 */
#include <stdio.h>
#include <stdlib.h>
#include "mem.h"

#define TOTAL       1024
#define OPERACOES   20000
#define AQUECIMENTO 2000
#define TAM_MAX     64
#define NSEMENTES   5

typedef struct {
    double falhas, frag, lacunas, minusculas, visitados;
} resultado_t;

static void mede(const memoria_t *m, double *frag, double *lacunas, double *minusculas) {
    unsigned livre = 0, maior = 0, n = 0, mini = 0;
    for (const segmento_t *s = m->cabeca; s; s = s->prox) {
        if (s->tipo != 'L') continue;
        n++;
        livre += s->tamanho;
        if (s->tamanho > maior) maior = s->tamanho;
        if (s->tamanho <= 2) mini++;
    }
    *frag += livre ? 1.0 - (double)maior / livre : 0.0;
    *lacunas += n;
    *minusculas += mini;
}

static resultado_t roda(algoritmo_t alg, unsigned semente) {
    memoria_t m;
    int vivos[TOTAL];               /* no maximo TOTAL processos de 1 unidade */
    int nvivos = 0, proximo_pid = 1;
    long pedidos = 0, falhas = 0, medidas = 0;
    unsigned long visitados0 = 0;
    double frag = 0, lacunas = 0, minusculas = 0;
    resultado_t r;

    srand(semente);
    mem_inicia(&m, TOTAL);

    for (int op = 0; op < OPERACOES; op++) {
        int conta = op >= AQUECIMENTO;
        if (op == AQUECIMENTO) visitados0 = m.visitados;

        if (nvivos == 0 || rand() % 100 < 55) {             /* alocacao */
            unsigned tam = 1 + rand() % TAM_MAX;
            int pid = proximo_pid++;
            if (mem_aloca(&m, pid, tam, alg) >= 0) vivos[nvivos++] = pid;
            else if (conta) falhas++;
            if (conta) pedidos++;
        } else {                                            /* liberacao */
            int i = rand() % nvivos;
            mem_libera(&m, vivos[i]);
            vivos[i] = vivos[--nvivos];
        }

        if (conta) {
            mede(&m, &frag, &lacunas, &minusculas);
            medidas++;
        }
    }

    r.falhas = 100.0 * falhas / pedidos;
    r.frag = frag / medidas;
    r.lacunas = lacunas / medidas;
    r.minusculas = minusculas / medidas;
    r.visitados = (double)(m.visitados - visitados0) / pedidos;
    mem_destroi(&m);
    return r;
}

int main(void) {
    const char *nome[] = { "First fit", "Next fit", "Best fit", "Worst fit" };

    printf("Media de %d sementes (1 a %d)\n\n", NSEMENTES, NSEMENTES);
    printf("Algoritmo   Falhas  Frag. externa  Lacunas  Lacunas 1-2  Visitados/pedido\n");
    for (int a = FIRST_FIT; a <= WORST_FIT; a++) {
        resultado_t soma = { 0, 0, 0, 0, 0 };
        for (unsigned s = 1; s <= NSEMENTES; s++) {
            resultado_t r = roda((algoritmo_t)a, s);
            soma.falhas += r.falhas;
            soma.frag += r.frag;
            soma.lacunas += r.lacunas;
            soma.minusculas += r.minusculas;
            soma.visitados += r.visitados;
        }
        printf("%-10s  %5.1f%%  %13.3f  %7.1f  %11.2f  %16.1f\n", nome[a],
               soma.falhas / NSEMENTES, soma.frag / NSEMENTES,
               soma.lacunas / NSEMENTES, soma.minusculas / NSEMENTES,
               soma.visitados / NSEMENTES);
    }
    return 0;
}
