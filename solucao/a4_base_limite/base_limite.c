/*
 * base_limite.c -- SOLUCAO (Atividade 4, Parte A)
 * Laboratorio de Gerencia de Memoria (Parte 1) -- slides 9 e 10
 *
 * Simula a traducao de enderecos feita pelo hardware com os registradores
 * base e limite. Neste roteiro, o registrador limite guarda o TAMANHO da
 * particao: o endereco logico e comparado com o limite e, se for valido,
 * somado a base. K = 1024 bytes.
 *
 * Compilar:  gcc -Wall -Wextra -g base_limite.c -o base_limite   (ou: make)
 * Executar:  ./base_limite
 */
#include <stdio.h>
#include <stdint.h>

#define K 1024u

typedef struct {
    uint32_t base;    /* inicio da particao na memoria fisica */
    uint32_t limite;  /* tamanho da particao                  */
} regs_t;

/* Retorna 0 e preenche *fisico, ou -1 se o acesso deve gerar uma interrupcao (trap). */
int traduz(const regs_t *r, uint32_t logico, uint32_t *fisico) {
    if (logico >= r->limite)        /* fora da particao: o hardware gera trap */
        return -1;
    *fisico = r->base + logico;     /* relocacao: soma a base */
    return 0;
}

/* O "programa": sequencia de enderecos logicos que ele acessa. */
static const uint32_t PROGRAMA[] = { 100, 5000, 99000, 150000 };
#define NEND (int)(sizeof PROGRAMA / sizeof PROGRAMA[0])

static void executa(const char *nome, regs_t r) {
    printf("%s (base %u, limite %u)\n", nome, r.base, r.limite);
    for (int i = 0; i < NEND; i++) {
        uint32_t fisico;
        if (traduz(&r, PROGRAMA[i], &fisico) == 0)
            printf("  logico %6u -> fisico %6u\n", PROGRAMA[i], fisico);
        else
            printf("  logico %6u -> TRAP: violacao de protecao\n", PROGRAMA[i]);
    }
    printf("\n");
}

int main(void) {
    executa("Particao 1", (regs_t){ 100 * K, 100 * K });
    executa("Particao 2", (regs_t){ 200 * K, 200 * K });
    return 0;
}
