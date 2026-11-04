/*
 * protecao.c -- CODIGO FORNECIDO (Atividade 4, Parte B)
 * Laboratorio de Gerencia de Memoria (Parte 1) -- slides 9 e 10
 *
 * Provoca violacoes de protecao reais e mostra o endereco que o hardware
 * barrou (o kernel entrega o erro ao processo como o sinal SIGSEGV).
 *
 *   ./protecao 1   escreve em um endereco que nao pertence ao processo
 *   ./protecao 2   escreve em uma area do processo que e somente leitura
 *   ./protecao 3   nao faz nenhum acesso invalido
 *
 * Compilar:  gcc -Wall -Wextra -g protecao.c -o protecao   (ou: make)
 */
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

static void trata(int sig, siginfo_t *info, void *ctx) {
    (void)ctx;
    char msg[128];
    int n = snprintf(msg, sizeof msg,
                     "SIGSEGV (sinal %d): acesso inválido ao endereço %p\n",
                     sig, info->si_addr);
    write(STDERR_FILENO, msg, n);
    _exit(1);
}

int main(int argc, char *argv[]) {
    struct sigaction sa;
    memset(&sa, 0, sizeof sa);
    sa.sa_sigaction = trata;
    sa.sa_flags = SA_SIGINFO;
    sigaction(SIGSEGV, &sa, NULL);

    int caso = argc > 1 ? atoi(argv[1]) : 1;
    if (caso == 1) {            /* endereço que não pertence ao processo */
        int *p = (int *)0x10;
        *p = 7;
    } else if (caso == 2) {     /* área do processo, mas somente leitura */
        char *s = "constante";
        s[0] = 'C';
    }
    puts("nenhuma violação");
    return 0;
}
