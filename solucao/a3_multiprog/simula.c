/*
 * simula.c -- SOLUCAO (Atividade 3, Passo 2)
 * Laboratorio de Gerencia de Memoria (Parte 1) -- slides 6 a 8
 *
 * Simula n processos: em cada tique, cada processo esta esperando E/S com
 * probabilidade p, de forma independente; a CPU esta ocupada no tique se
 * pelo menos um processo NAO estiver esperando. Compara a utilizacao
 * medida com a do modelo 1 - p^n.
 *
 * Compilar:  gcc -Wall -Wextra -g simula.c -o simula -lm   (ou: make)
 * Executar:  ./simula
 */
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define TIQUES 200000L
#define NMAX   10

static const double P[] = { 0.2, 0.5, 0.8 };
#define NP (int)(sizeof P / sizeof P[0])

/* Devolve a fracao de tiques em que a CPU ficou ocupada. */
static double simula(double p, int n) {
    long ocupados = 0;
    for (long t = 0; t < TIQUES; t++) {
        int alguem_pronto = 0;
        for (int i = 0; i < n && !alguem_pronto; i++)
            if ((double)rand() / RAND_MAX >= p) alguem_pronto = 1;
        ocupados += alguem_pronto;
    }
    return (double)ocupados / TIQUES;
}

int main(void) {
    double pior = 0.0;

    srand(1);
    printf("   p   n   medida   modelo   diferenca\n");
    for (int j = 0; j < NP; j++) {
        for (int n = 0; n <= NMAX; n++) {
            double med = simula(P[j], n);
            double mod = 1.0 - pow(P[j], n);
            printf("%4.1f %3d  %7.4f  %7.4f  %+8.4f\n", P[j], n, med, mod, med - mod);
            if (fabs(med - mod) > pior) pior = fabs(med - mod);
        }
    }
    printf("\nmaior diferenca absoluta: %.4f\n", pior);
    return 0;
}
