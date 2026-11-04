/*
 * mapa.c -- CODIGO FORNECIDO (Atividade 1)
 * Laboratorio de Gerencia de Memoria (Parte 1) -- slides 2, 3 e 13
 *
 * Imprime o endereco de uma variavel em cada regiao do espaco de
 * enderecamento (codigo, dados, BSS, heap e pilha) e fica parado no
 * getchar() para que voce inspecione /proc/<PID>/maps em outro terminal.
 *
 * Compilar:  gcc -Wall -Wextra -g mapa.c -o mapa   (ou: make)
 * Executar:  ./mapa
 *            setarch $(uname -m) -R ./mapa          (sem ASLR)
 */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int global_inicializada = 42;      /* segmento de dados */
int global_nao_inicializada;       /* BSS */

void mostra_pilha(int nivel) {
    int local;
    printf("  pilha (nível %d): %p\n", nivel, (void *)&local);
    if (nivel < 3) mostra_pilha(nivel + 1);
}

int main(void) {
    printf("código   (main)          : %p\n", (void *)main);
    printf("dados    (inicializada)  : %p\n", (void *)&global_inicializada);
    printf("BSS      (não inicial.)  : %p\n", (void *)&global_nao_inicializada);
    for (int i = 0; i < 3; i++) {
        void *p = malloc(64 * 1024);
        printf("  heap (malloc %d)       : %p\n", i, p);
    }
    mostra_pilha(0);
    printf("\nPID %d — tecle Enter para encerrar (veja /proc/%d/maps em outro terminal)\n",
           getpid(), getpid());
    getchar();
    return 0;
}
