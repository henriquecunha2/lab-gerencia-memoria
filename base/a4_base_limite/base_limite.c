/*
 * base_limite.c -- CODIGO BASE (Atividade 4, Parte A)
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
    /* TODO 1: compare o endereco logico com o limite; se for valido,
       calcule o endereco fisico somando a base. */
    (void)r; (void)logico; (void)fisico;
    return -1;
}

/* O "programa": sequencia de enderecos logicos que ele acessa. */
static const uint32_t PROGRAMA[] = { 100, 5000, 99000, 150000 };
#define NEND (int)(sizeof PROGRAMA / sizeof PROGRAMA[0])

static void executa(const char *nome, regs_t r) {
    printf("%s (base %u, limite %u)\n", nome, r.base, r.limite);
    for (int i = 0; i < NEND; i++) {
        uint32_t fisico;
        /* TODO 2: chame traduz e imprima
             "  logico   100 -> fisico 102500"
           ou
             "  logico 150000 -> TRAP: violacao de protecao" */
        (void)fisico;
    }
    printf("\n");
}

int main(void) {
    executa("Particao 1", (regs_t){ 100 * K, 100 * K });
    /* TODO 3: execute o mesmo programa carregado na particao 2
       (base 200K, limite 200K). */
    return 0;
}
