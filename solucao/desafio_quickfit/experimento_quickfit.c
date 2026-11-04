/*
 * experimento_quickfit.c -- SOLUCAO (Desafio -- quick fit, Passos 2 e 3)
 * Laboratorio de Gerencia de Memoria (Parte 1) -- slide 18
 *
 * Mesma carga da Atividade 8 (1024 unidades, 20 000 operacoes, 2 000 de
 * aquecimento, 55% alocacao, sementes 1 a 5), mas com tamanhos:
 *   80% sorteados em {4, 8, 16, 32, 64} e 20% uniformes entre 1 e 64.
 * Compara FIRST_FIT (mem.c da Atividade 7/8) com QUICK_FIT sem fusao e
 * com fusao a cada N liberacoes.
 *
 * Compilar:  gcc -Wall -Wextra -O2 -I../a7_a8_lista experimento_quickfit.c \
 *                quickfit.c ../a7_a8_lista/mem.c -o experimento_quickfit   (ou: make)
 */
#include <stdio.h>
#include <stdlib.h>
#include "quickfit.h"
#include "mem.h"

#define TOTAL       1024
#define OPERACOES   20000
#define AQUECIMENTO 2000
#define NSEMENTES   5

typedef struct { unsigned inicio, tam; int pid; } vivo_t;

typedef struct {
    double falhas, frag, lacunas, visitados, fusao_por_op;
} resultado_t;

static unsigned sorteia_tam(void) {
    static const unsigned EX[] = { 4, 8, 16, 32, 64 };
    if (rand() % 100 < 80) return EX[rand() % 5];
    return 1 + rand() % 64;
}

static double frag_de(unsigned livre, unsigned maior) {
    return livre ? 1.0 - (double)maior / livre : 0.0;
}

/* intervalo < 0 -> FIRST_FIT; 0 -> QUICK_FIT sem fusao; > 0 -> com fusao */
static resultado_t roda(int intervalo, unsigned semente) {
    memoria_t m;
    quickfit_t q;
    vivo_t vivos[TOTAL];
    int nvivos = 0, proximo_pid = 1, ff = intervalo < 0;
    long pedidos = 0, falhas = 0, medidas = 0;
    unsigned long vis0 = 0, fus0 = 0;
    double frag = 0, lacunas = 0;
    resultado_t r;

    srand(semente);
    if (ff) mem_inicia(&m, TOTAL);
    else    qf_inicia(&q, TOTAL, (unsigned)intervalo);

    for (int op = 0; op < OPERACOES; op++) {
        int conta = op >= AQUECIMENTO;
        if (op == AQUECIMENTO) {
            vis0 = ff ? m.visitados : q.visitados;
            fus0 = ff ? 0 : q.examinados_fusao;
        }

        if (nvivos == 0 || rand() % 100 < 55) {
            unsigned tam = sorteia_tam();
            int pid = proximo_pid++;
            int ini = ff ? mem_aloca(&m, pid, tam, FIRST_FIT) : qf_aloca(&q, tam);
            if (ini >= 0) vivos[nvivos++] = (vivo_t){ (unsigned)ini, tam, pid };
            else if (conta) falhas++;
            if (conta) pedidos++;
        } else {
            int i = rand() % nvivos;
            if (ff) mem_libera(&m, vivos[i].pid);
            else    qf_libera(&q, vivos[i].inicio, vivos[i].tam);
            vivos[i] = vivos[--nvivos];
        }

        if (conta) {
            unsigned livre = 0, maior = 0, n = 0;
            if (ff) {
                for (const segmento_t *s = m.cabeca; s; s = s->prox)
                    if (s->tipo == 'L') {
                        n++; livre += s->tamanho;
                        if (s->tamanho > maior) maior = s->tamanho;
                    }
            } else {
                qf_mede(&q, &livre, &maior, &n);
            }
            frag += frag_de(livre, maior);
            lacunas += n;
            medidas++;
        }
    }

    r.falhas = 100.0 * falhas / pedidos;
    r.frag = frag / medidas;
    r.lacunas = lacunas / medidas;
    r.visitados = (double)((ff ? m.visitados : q.visitados) - vis0) / pedidos;
    r.fusao_por_op = ff ? 0 : (double)(q.examinados_fusao - fus0) / (OPERACOES - AQUECIMENTO);
    if (ff) mem_destroi(&m); else qf_destroi(&q);
    return r;
}

int main(void) {
    const int configs[] = { -1, 0, 25, 50, 100, 200, 500, 1000 };
    const int nconf = (int)(sizeof configs / sizeof configs[0]);

    printf("Media de %d sementes; 80%% dos pedidos em {4,8,16,32,64}\n\n", NSEMENTES);
    printf("Configuracao             Falhas  Frag. ext.  Lacunas  Visit./pedido  Fusao: examin./op\n");
    for (int c = 0; c < nconf; c++) {
        resultado_t soma = { 0, 0, 0, 0, 0 };
        char nome[40];
        for (unsigned s = 1; s <= NSEMENTES; s++) {
            resultado_t r = roda(configs[c], s);
            soma.falhas += r.falhas; soma.frag += r.frag; soma.lacunas += r.lacunas;
            soma.visitados += r.visitados; soma.fusao_por_op += r.fusao_por_op;
        }
        if (configs[c] < 0)       snprintf(nome, sizeof nome, "FIRST_FIT");
        else if (configs[c] == 0) snprintf(nome, sizeof nome, "QUICK_FIT sem fusao");
        else                      snprintf(nome, sizeof nome, "QUICK_FIT fusao/%d", configs[c]);
        printf("%-23s  %5.1f%%  %10.3f  %7.1f  %13.1f  %17.2f\n", nome,
               soma.falhas / NSEMENTES, soma.frag / NSEMENTES, soma.lacunas / NSEMENTES,
               soma.visitados / NSEMENTES, soma.fusao_por_op / NSEMENTES);
    }
    return 0;
}
