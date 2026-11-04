/*
 * modelo.c -- SOLUCAO (Atividade 3, Passo 1)
 * Laboratorio de Gerencia de Memoria (Parte 1) -- slides 6 a 8
 *
 * Imprime a utilizacao da CPU prevista pelo modelo probabilistico
 *     utilizacao = 1 - p^n
 * para n = 0..10 (linhas) e p = 0,2; 0,5; 0,8 (colunas).
 *
 * Compilar:  gcc -Wall -Wextra -g modelo.c -o modelo -lm   (ou: make)
 * Executar:  ./modelo > modelo.txt ; python3 grafico.py modelo.txt
 */
#include <stdio.h>
#include <math.h>

#define NMAX 10

static const double P[] = { 0.2, 0.5, 0.8 };
#define NP (int)(sizeof P / sizeof P[0])

int main(void) {
    printf("# n");
    for (int j = 0; j < NP; j++) printf("   p=%.1f", P[j]);
    printf("\n");

    for (int n = 0; n <= NMAX; n++) {
        printf("%3d", n);
        for (int j = 0; j < NP; j++) {
            double u = 1.0 - pow(P[j], n);
            printf("  %7.4f", u);
        }
        printf("\n");
    }
    return 0;
}
