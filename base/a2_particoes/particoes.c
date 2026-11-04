/*
 * particoes.c -- CODIGO FORNECIDO (Atividade 2)
 * Laboratorio de Gerencia de Memoria (Parte 1) -- slides 4 e 5
 *
 * Simula a multiprogramacao com particoes fixas nas duas organizacoes
 * do slide 5, usando as MESMAS particoes e os MESMOS jobs:
 *
 *   Politica A -- multiplas filas (slide 5a): cada job entra, no instante
 *                 da chegada, na fila da menor particao que o comporta, e
 *                 cada particao atende apenas a propria fila.
 *   Politica B -- fila unica (slide 5b): o job da frente da fila ocupa a
 *                 menor particao LIVRE que o comporta; se nao couber em
 *                 nenhuma livre, a fila inteira espera.
 *
 * O tempo e discreto (tiques). Em cada tique, primeiro as particoes cujos
 * jobs terminaram sao liberadas e depois os jobs que esperam sao alocados.
 *
 * Voce NAO precisa alterar este arquivo: compile, execute e use a saida
 * para responder as perguntas Q2.x do roteiro.
 *
 * Compilar:  gcc -Wall -Wextra -g particoes.c -o particoes   (ou: make)
 * Executar:  ./particoes
 */

#include <stdio.h>
#include <string.h>

/* ------------------------------------------------------------------ */
/* Cenario (em K). O SO ocupa de 0K a 100K.                            */
/* ------------------------------------------------------------------ */

#define NPART 4
#define NJOBS 8
#define TMAX  64            /* limite de seguranca para a simulacao */

typedef struct {
    const char *nome;
    int inicio;             /* endereco inicial (K) */
    int tam;                /* tamanho (K)          */
} particao_t;

typedef struct {
    const char *nome;
    int tam;                /* tamanho (K)                 */
    int dur;                /* duracao (tiques)            */
    /* resultados da simulacao */
    int part;               /* indice da particao usada    */
    int inicio, fim;        /* instante de inicio e de fim */
} job_t;

static const particao_t PARTICOES[NPART] = {
    { "P1", 100, 100 },
    { "P2", 200, 200 },
    { "P3", 400, 300 },
    { "P4", 700, 100 },
};

/* Todos chegam no instante 0, nesta ordem. */
static const job_t JOBS[NJOBS] = {
    { "J1",  80, 4, -1, -1, -1 },
    { "J2",  90, 3, -1, -1, -1 },
    { "J3",  60, 5, -1, -1, -1 },
    { "J4", 250, 2, -1, -1, -1 },
    { "J5",  95, 2, -1, -1, -1 },
    { "J6",  30, 6, -1, -1, -1 },
    { "J7", 180, 3, -1, -1, -1 },
    { "J8",  70, 2, -1, -1, -1 },
};

/* ------------------------------------------------------------------ */
/* Estado da simulacao                                                 */
/* ------------------------------------------------------------------ */

typedef struct {
    job_t jobs[NJOBS];
    int ocupante[NPART];        /* indice do job na particao, ou -1   */
    int linha[NPART][TMAX];     /* linha do tempo: job em cada tique  */
    int terminados;
} sim_t;

static void sim_inicia(sim_t *s) {
    memcpy(s->jobs, JOBS, sizeof JOBS);
    for (int p = 0; p < NPART; p++) {
        s->ocupante[p] = -1;
        for (int t = 0; t < TMAX; t++) s->linha[p][t] = -1;
    }
    s->terminados = 0;
}

/* Libera as particoes cujos jobs terminam no instante t. */
static void libera_terminados(sim_t *s, int t) {
    for (int p = 0; p < NPART; p++) {
        int j = s->ocupante[p];
        if (j >= 0 && s->jobs[j].fim == t) {
            s->ocupante[p] = -1;
            s->terminados++;
        }
    }
}

/* Coloca o job j na particao p no instante t. */
static void aloca(sim_t *s, int j, int p, int t) {
    s->ocupante[p] = j;
    s->jobs[j].part = p;
    s->jobs[j].inicio = t;
    s->jobs[j].fim = t + s->jobs[j].dur;
    for (int k = t; k < s->jobs[j].fim && k < TMAX; k++) s->linha[p][k] = j;
}

/* ------------------------------------------------------------------ */
/* Politica A -- multiplas filas                                       */
/* ------------------------------------------------------------------ */

static void politica_a(sim_t *s) {
    int fila[NPART][NJOBS];     /* fila de cada particao (FIFO) */
    int tam_fila[NPART] = {0};
    int frente[NPART] = {0};

    /* No instante 0, cada job escolhe a sua fila: a menor particao que o
       comporta; empate de tamanho -> fila mais curta; persistindo o
       empate -> particao de menor numero. */
    for (int j = 0; j < NJOBS; j++) {
        int escolhida = -1;
        for (int p = 0; p < NPART; p++) {
            if (PARTICOES[p].tam < s->jobs[j].tam) continue;
            if (escolhida < 0
                || PARTICOES[p].tam < PARTICOES[escolhida].tam
                || (PARTICOES[p].tam == PARTICOES[escolhida].tam
                    && tam_fila[p] < tam_fila[escolhida]))
                escolhida = p;
        }
        fila[escolhida][tam_fila[escolhida]++] = j;
    }

    printf("Filas formadas no instante 0:\n");
    for (int p = 0; p < NPART; p++) {
        printf("  %s (%3dK):", PARTICOES[p].nome, PARTICOES[p].tam);
        if (tam_fila[p] == 0) printf(" (vazia)");
        for (int i = 0; i < tam_fila[p]; i++) printf(" %s", s->jobs[fila[p][i]].nome);
        printf("\n");
    }
    printf("\n");

    /* Cada particao atende apenas a propria fila, em ordem de chegada. */
    for (int t = 0; s->terminados < NJOBS && t < TMAX; t++) {
        libera_terminados(s, t);
        for (int p = 0; p < NPART; p++)
            if (s->ocupante[p] < 0 && frente[p] < tam_fila[p])
                aloca(s, fila[p][frente[p]++], p, t);
    }
}

/* ------------------------------------------------------------------ */
/* Politica B -- fila unica                                            */
/* ------------------------------------------------------------------ */

/* Menor particao livre que comporta o job j, ou -1. */
static int menor_livre(const sim_t *s, int j) {
    int escolhida = -1;
    for (int p = 0; p < NPART; p++) {
        if (s->ocupante[p] >= 0 || PARTICOES[p].tam < s->jobs[j].tam) continue;
        if (escolhida < 0 || PARTICOES[p].tam < PARTICOES[escolhida].tam)
            escolhida = p;
    }
    return escolhida;
}

static void politica_b(sim_t *s) {
    int frente = 0;             /* a fila unica e a propria ordem dos jobs */

    for (int t = 0; s->terminados < NJOBS && t < TMAX; t++) {
        libera_terminados(s, t);
        /* Enquanto o job da frente couber em alguma particao livre... */
        while (frente < NJOBS) {
            int p = menor_livre(s, frente);
            if (p < 0) break;   /* ...senao, a fila inteira espera. */
            aloca(s, frente, p, t);
            frente++;
        }
    }
}

/* ------------------------------------------------------------------ */
/* Relatorio                                                           */
/* ------------------------------------------------------------------ */

static void relatorio(const sim_t *s) {
    int ultimo = 0, espera_total = 0, frag_total = 0;

    printf("Job  Tamanho  Particao  Inicio  Fim  Espera  Frag. interna\n");
    printf("---  -------  --------  ------  ---  ------  -------------\n");
    for (int j = 0; j < NJOBS; j++) {
        const job_t *jb = &s->jobs[j];
        const particao_t *pt = &PARTICOES[jb->part];
        int espera = jb->inicio;            /* todos chegam em t = 0 */
        int frag = pt->tam - jb->tam;
        printf("%-3s  %5dK   %s %3dK   %4d   %3d  %5d   %5dK\n",
               jb->nome, jb->tam, pt->nome, pt->tam,
               jb->inicio, jb->fim, espera, frag);
        if (jb->fim > ultimo) ultimo = jb->fim;
        espera_total += espera;
        frag_total += frag;
    }

    printf("\nLinha do tempo (cada coluna e um tique; o digito e o job em execucao;\n"
           "'.' = particao livre)\n");
    printf("              t: ");
    for (int t = 0; t < ultimo; t++) printf("%d", t % 10);
    printf("\n");
    for (int p = 0; p < NPART; p++) {
        int ocupados = 0;
        printf("  %s %3dK-%3dK : ", PARTICOES[p].nome, PARTICOES[p].inicio,
               PARTICOES[p].inicio + PARTICOES[p].tam);
        for (int t = 0; t < ultimo; t++) {
            int j = s->linha[p][t];
            if (j >= 0) { printf("%d", j + 1); ocupados++; }
            else printf(".");
        }
        printf("   ocupada %2d de %2d tiques\n", ocupados, ultimo);
    }

    printf("\nInstante em que o ultimo job termina : %d tiques\n", ultimo);
    printf("Espera media                         : %.3f tiques\n",
           (double)espera_total / NJOBS);
    printf("Fragmentacao interna total           : %dK\n", frag_total);
}

int main(void) {
    sim_t s;

    printf("Particoes fixas (SO de 0K a 100K):");
    for (int p = 0; p < NPART; p++)
        printf("  %s=%dK", PARTICOES[p].nome, PARTICOES[p].tam);
    printf("\nJobs (todos chegam em t = 0):");
    for (int j = 0; j < NJOBS; j++)
        printf("  %s=%dK/%dt", JOBS[j].nome, JOBS[j].tam, JOBS[j].dur);
    printf("\n\n");

    printf("==================== Politica A -- multiplas filas (slide 5a) ====================\n\n");
    sim_inicia(&s);
    politica_a(&s);
    relatorio(&s);

    printf("\n==================== Politica B -- fila unica (slide 5b) =========================\n\n");
    sim_inicia(&s);
    politica_b(&s);
    relatorio(&s);

    return 0;
}
