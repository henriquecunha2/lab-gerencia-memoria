# Roteiro de Laboratório — Gerência de Memória (Parte 1)

## Informações gerais

Neste laboratório você vai observar, simular e implementar em C os mecanismos de gerência de memória vistos nos slides 2 a 19: monoprogramação, partições fixas, grau de multiprogramação, relocação e proteção, troca de processos, mapas de bits, listas encadeadas e os algoritmos *first fit*, *next fit*, *best fit*, *worst fit* e *quick fit*.

| Item | Descrição |
| --- | --- |
| Disciplina | Sistemas Operacionais — Engenharia de Computação, IFPB *Campus* Campina Grande |
| Conteúdo | Gerência de Memória, slides 2 a 19 (Fundamentos; Relocação, proteção e troca; Gerência de espaço livre) |
| Duração estimada | 2 encontros de laboratório de 2 h (Atividades 1 a 5 no primeiro; 6 a 8 no segundo) |
| Pré-requisitos | Linux (nativo, VM ou WSL), C com ponteiros e `struct`, `gcc` e `make` |
| Trabalho | Em dupla; cada dupla mantém um repositório com o código e um `RELATORIO.md` |

**Objetivos.** Ao final, você deve ser capaz de:

1. Ler o mapa de memória de um processo Linux e relacioná-lo aos esquemas de organização da memória.
2. Medir a fragmentação interna de partições fixas e comparar fila única e múltiplas filas.
3. Calcular a utilização da CPU pelo modelo 1 − pⁿ e validá-la por simulação.
4. Implementar a tradução de endereços com registradores base e limite.
5. Implementar um gerenciador de espaço livre com mapa de bits e com lista encadeada.
6. Implementar e comparar os algoritmos de alocação, justificando qual se sai melhor em cada métrica.

As perguntas marcadas com **Q** devem ser respondidas no `RELATORIO.md`, com a saída do programa que fundamenta cada resposta.

## Preparação do ambiente

Use uma distribuição Linux com `gcc`, `make` e `python3` (ou `gnuplot`) instalados; o WSL 2 funciona para todas as atividades. Para a Atividade 7, instale também o `valgrind`.

Cada dupla faz um *fork* deste repositório (ou o clona e envia para um repositório próprio) e trabalha na pasta `base/`, que já tem a estrutura abaixo. **Todos os caminhos citados no roteiro são relativos a `base/`.** Crie o `RELATORIO.md` na raiz do repositório.

```sh
git clone https://github.com/henriquecunha2/lab-gerencia-memoria.git
cd lab-gerencia-memoria
make base        # compila o código base
```

```
lab-gerencia-memoria/
├── Makefile                <- make, make base, make solucao, make clean
├── RELATORIO.md            <- criado pela dupla
├── base/                   <- onde você trabalha
│   ├── a1_mapa/            # Atividade 1: mapa.c (fornecido)
│   ├── a2_particoes/       # Atividade 2: particoes.c (fornecido; só executar)
│   ├── a3_multiprog/       # Atividade 3: modelo.c e simula.c (TODOs)
│   ├── a4_base_limite/     # Atividade 4: base_limite.c (TODOs) e protecao.c (fornecido)
│   ├── a5_troca/           # Atividade 5: troca.sh
│   ├── a6_bitmap/          # Atividade 6: bitmap.h, bitmap.c e bench.c (TODOs), teste_slide14.c
│   └── a7_a8_lista/        # Atividades 7 e 8 (mesmo código): mem.h, mem.c (TODOs), teste_slide14.c
└── solucao/                <- implementação de referência
```

Os programas que você criar nas Atividades 7 e 8 (`teste_casos.c`, `troca.c`, `slide19.c`, `experimento.c`) já têm regra no `Makefile`; acrescente-os à lista `BASE` quando existirem. Consulte `solucao/` só depois de tentar.

Compile sempre com avisos ligados: `gcc -Wall -Wextra -g`. Nas atividades 6 a 8, a unidade de alocação é abstrata: todos os tamanhos são em unidades, como nos slides 14 e 16.

## Atividade 1 — O mapa de memória de um processo (slides 2, 3 e 13)

O gerenciador de memória controla quais partes estão em uso, aloca, desaloca e troca processos (slide 2). Antes de simular esses papéis, vamos ver como o Linux organiza a memória de um processo real.

**Passo 1.** O programa abaixo já está em `a1_mapa/mapa.c`. Compile e execute:

```c
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
```

**Passo 2.** Com o programa parado no `getchar()`, abra outro terminal e rode `cat /proc/<PID>/maps`. Localize as linhas do executável, `[heap]`, as bibliotecas compartilhadas, `[stack]` e `[vdso]`.

**Passo 3.** Execute o programa três vezes seguidas e compare os endereços. Depois repita com `setarch $(uname -m) -R ./mapa`, que desliga a aleatorização do espaço de endereçamento (ASLR).

**Q1.1** Monte um desenho do espaço de endereçamento no estilo do slide 13(b): da base para o topo, onde ficam código, dados, BSS, heap e pilha? Em que sentido cada região cresce, segundo a sua saída?

**Q1.2** O slide 3 mostra três organizações de monoprogramação. Em quais delas o SO fica em endereços baixos e o programa acima dele? Por que o Linux, que é multiprogramado, consegue dar a *cada* processo a ilusão de ter a memória só para si?

**Q1.3** Os endereços mudam a cada execução no Passo 3, mas o programa funciona do mesmo jeito. Que problema do slide 9 isso exige que esteja resolvido? Guarde a resposta: ela volta na Atividade 4.

## Atividade 2 — Partições fixas, filas de entrada e fragmentação interna (slides 4–5)

Nesta atividade você não programa: o simulador `a2_particoes/particoes.c` já vem pronto. Você vai executá-lo, ler a saída e tirar conclusões sobre as duas organizações de partições fixas do slide 5, que aqui usam as mesmas partições e os mesmos *jobs*.

Lembre como funciona a multiprogramação com partições fixas (slide 4): as partições são definidas na inicialização do sistema e não mudam de tamanho; cada *job* ocupa uma partição inteira, e a parte da partição que ele não usa fica perdida enquanto ele executa. Essa sobra é a **fragmentação interna**. O simulador mede, para cada organização, três coisas: quando a carga inteira termina, quanto cada *job* espera para começar e quanta memória se perde por fragmentação interna.

**Cenário.** O SO ocupa de 0K a 100K. As partições são as do slide 5:

| Partição | Faixa | Tamanho |
| --- | --- | --- |
| P1 | 100K–200K | 100K |
| P2 | 200K–400K | 200K |
| P3 | 400K–700K | 300K |
| P4 | 700K–800K | 100K |

Todos os *jobs* chegam no instante 0, nesta ordem. O tempo é discreto (tiques).

| *Job* | Tamanho | Duração (tiques) |
| --- | --- | --- |
| J1 | 80K | 4 |
| J2 | 90K | 3 |
| J3 | 60K | 5 |
| J4 | 250K | 2 |
| J5 | 95K | 2 |
| J6 | 30K | 6 |
| J7 | 180K | 3 |
| J8 | 70K | 2 |

O simulador implementa as duas políticas com as regras abaixo. O tempo é discreto e, em cada tique, primeiro são liberadas as partições cujos *jobs* terminaram e depois são alocados os *jobs* que esperam.

**Política A — múltiplas filas (slide 5a).** Cada *job* entra na fila da menor partição que o comporta (slide 4); se houver empate de tamanho, entra na fila mais curta naquele momento e, persistindo o empate, na de menor número. Cada partição atende só a própria fila, em ordem de chegada.

**Política B — fila única (slide 5b).** A cada tique, enquanto o *job* da frente da fila couber em alguma partição livre, ele ocupa a menor partição livre que o comporta. Se não couber em nenhuma livre, a fila inteira espera, mesmo que outro *job* atrás dele coubesse.

**Passo 1.** Na pasta `a2_particoes`, compile e execute o simulador, guardando a saída em um arquivo:

```sh
gcc -Wall -Wextra -g particoes.c -o particoes
./particoes | tee saida_a2.txt
```

Não altere o código. Se quiser ver como cada regra foi implementada, leia as funções `politica_a` e `politica_b`: cada uma tem poucas linhas e segue exatamente a descrição acima.

**Passo 2.** Leia a saída. Para cada política, o programa imprime, nesta ordem:

- as filas formadas no instante 0 (só na Política A);
- uma linha por *job*: partição usada, instantes de início e de fim, espera (início − chegada; como todos chegam em 0, é o próprio início) e fragmentação interna (tamanho da partição − tamanho do *job*);
- a linha do tempo: uma linha por partição e uma coluna por tique, em que o dígito é o *job* em execução e `.` indica partição livre; ao lado, quantos tiques a partição ficou ocupada;
- o resumo: instante em que o último *job* termina, espera média e fragmentação interna total.

Cole a saída completa no RELATORIO.md e use-a para responder às perguntas abaixo, citando em cada resposta as linhas que a sustentam.

**Q2.1** Compare os resumos das duas políticas. Qual termina antes e qual tem a menor espera média? Qual desperdiça mais memória com fragmentação interna? Explique a troca observada: o que a Política B faz para ganhar tempo e o que isso custa em memória.

**Q2.2** Na linha do tempo da Política A, encontre partições livres (`.`) enquanto ainda havia *jobs* esperando em outra fila. Quais partições, a partir de que tique, e quais *jobs* esperavam? Relacione com a legenda do slide 5(a).

**Q2.3** Na Política B, por que J7 (180K) só começa no tique 4? Que *jobs* ocupavam as partições em que ele caberia, e quanto cada um desperdiçava? Proponha uma regra de escolha do próximo *job* que reduza esse efeito e diga que risco ela traz, e para quais *jobs*.

**Q2.4** A fragmentação interna total soma a sobra de cada *job*, sem considerar por quanto tempo ela durou. Usando a tabela da saída, calcule para cada política a soma de (fragmentação interna × duração), em K·tique, e divida pelo instante do último término para obter a memória desperdiçada, em média, a cada tique. A conclusão da Q2.1 se mantém?

## Atividade 3 — Modelo probabilístico de multiprogramação (slides 6–8)

Você vai reproduzir o gráfico do slide 8 pela fórmula e confirmar a fórmula por simulação.

```latex
\text{utilização da CPU} = 1 - p^{n}
```

**Passo 1.** Em `a3_multiprog/modelo.c`, imprima uma tabela com `n` de 0 a 10 e uma coluna para cada `p` ∈ {0,2; 0,5; 0,8}. Compile com `-lm`. Gere o gráfico com `gnuplot` ou `matplotlib` e compare com o slide 8.

**Passo 2.** Em `a3_multiprog/simula.c`, simule o sistema: em cada tique, cada um dos `n` processos está esperando E/S com probabilidade `p`, de forma independente; a CPU está ocupada no tique se pelo menos um processo não estiver esperando. Rode 200 000 tiques para cada par (p, n) e imprima, lado a lado, a utilização medida e a do modelo.

```c
/* esqueleto do laço de simulação */
long ocupados = 0;
for (long t = 0; t < TIQUES; t++) {
    int alguem_pronto = 0;
    for (int i = 0; i < n && !alguem_pronto; i++)
        if ((double)rand() / RAND_MAX >= p) alguem_pronto = 1;
    ocupados += alguem_pronto;
}
```

**Q3.1** Confirme o valor do slide 7 (p = 0,8 e n = 5). Quanto a simulação se afastou do modelo, no pior caso da sua tabela?

**Q3.2** Com p = 0,8, qual o menor n que garante pelo menos 90% de utilização? Mostre a conta com logaritmos e confira na tabela.

**Q3.3** Um computador tem 512 MB de memória; o SO ocupa 128 MB e cada processo de usuário ocupa 128 MB, com p = 0,8. Qual a utilização da CPU? Se forem comprados mais 512 MB, qual a nova utilização? Use o resultado para argumentar quando vale a pena comprar memória.

**Q3.4** A simulação sorteia cada processo de forma independente, exatamente como o modelo supõe. Cite uma situação real em que essa hipótese falha e diga se a utilização real ficaria acima ou abaixo da prevista.

## Atividade 4 — Relocação e proteção com base e limite (slides 9–10)

Você vai implementar a tradução feita pelo *hardware* com registradores base e limite e, em seguida, provocar a proteção real do Linux.

**Parte A — simulador.** Use K = 1024 bytes. Neste roteiro, o registrador limite guarda o **tamanho** da partição: o endereço lógico é comparado com o limite e, se for válido, somado à base.

```c
#include <stdint.h>

typedef struct {
    uint32_t base;    /* início da partição na memória física */
    uint32_t limite;  /* tamanho da partição                  */
} regs_t;

/* Retorna 0 e preenche *fisico, ou -1 se o acesso deve gerar uma interrupção (trap). */
int traduz(const regs_t *r, uint32_t logico, uint32_t *fisico);
```

**Passo 1.** Implemente `traduz` em `a4_base_limite/base_limite.c`. O "programa" é a sequência de endereços lógicos `100, 5000, 99000, 150000`; o primeiro corresponde à chamada ao endereço absoluto 100 do slide 9.

**Passo 2.** Execute o mesmo programa carregado na partição 1 (base 100K, limite 100K) e na partição 2 (base 200K, limite 200K), usando as partições da Atividade 2. Imprima cada tradução ou a mensagem `TRAP: violação de proteção`.

**Parte B — proteção real.** O programa abaixo já está em `a4_base_limite/protecao.c`. Compile e execute com os argumentos `1`, `2` e `3`:

```c
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
```

**Q4.1** Qual endereço do programa gera *trap* em uma partição e não na outra? Por quê?

**Q4.2** Uma alternativa à base é a relocação feita pelo carregador: ao carregar o programa, ele soma o endereço da partição a cada endereço do código. Compare essa solução com o registrador base quando o processo é trocado e volta em outro endereço (slide 11, quadro g).

**Q4.3** O slide 10 diz que a desvantagem é uma adição e uma comparação a cada acesso. Por que isso precisa ser feito em *hardware* e não pelo SO? E por que o usuário não pode alterar os registradores?

**Q4.4** No caso `2` da Parte B, o endereço pertence ao processo e, mesmo assim, o acesso é barrado. O que o *hardware* precisa saber além de base e limite para barrar esse acesso? Retome a Q1.3: por que o ASLR depende de relocação?

## Atividade 5 — Troca de processos, lacunas e crescimento (slides 11–13)

Você vai reproduzir à mão a sequência do slide 11 com tamanhos concretos e ver por que a volta de um processo exige relocação.

**Cenário.** Memória de 1024K; o SO ocupa de 0K a 128K. Os processos têm A = 192K, B = 128K, C = 192K e D = 128K. Cada processo que entra ocupa a primeira lacuna, a partir do endereço mais baixo, grande o bastante para ele.

**Passo 1.** Preencha a tabela no `RELATORIO.md`, registrando a memória como lista no formato do slide 14 (tipo · início · tamanho, em K):

| Quadro | Evento | Memória depois do evento |
| --- | --- | --- |
| (a) | A entra |  |
| (b) | B entra |  |
| (c) | C entra |  |
| (d) | A é levado ao disco |  |
| (e) | D entra |  |
| (f) | B termina |  |
| (g) | A volta do disco |  |

Quando terminar a Atividade 7, confira a tabela executando a mesma sequência com o seu código.

**Passo 2.** Rode `free -h` e `swapon --show` na sua máquina e anote o tamanho da área de troca. O Linux troca páginas, não processos inteiros; esse mecanismo é o assunto da próxima parte da disciplina.

**Q5.1** Em que endereço A estava no quadro (a) e em que endereço voltou no (g)? Que valor o registrador base de A deve ter em cada caso?

**Q5.2** No quadro (e) sobra uma lacuna pequena. Quantas lacunas existem em (e) e em (f)? Por que a saída de B não aumentou esse número?

**Q5.3** Uma máquina tem 8 GB de memória e copia 8 bytes a cada 4 ns. Estime o tempo, no pior caso, para compactar a memória inteira (slide 12). O que isso diz sobre compactar a cada término de processo?

**Q5.4** Usando a saída da Atividade 1, mostre que heap e pilha crescem um em direção ao outro, como no slide 13(b). Que vantagem essa disposição tem sobre reservar uma área de crescimento separada para cada região, como no slide 13(a)?

## Atividade 6 — Mapa de bits (slides 14–15)

Você vai implementar um gerenciador com mapa de bits, reproduzir o slide 14 e medir o custo de procurar *k* zeros consecutivos.

```c
#include <stdint.h>

typedef struct {
    uint8_t *bits;      /* 1 bit por unidade; 1 = ocupada */
    unsigned unidades;  /* quantidade de unidades         */
} bitmap_t;

void bm_inicia(bitmap_t *b, unsigned unidades);
void bm_marca(bitmap_t *b, unsigned inicio, unsigned tam, int ocupado);
int  bm_procura(const bitmap_t *b, unsigned k);   /* 1ª sequência de k zeros, ou -1 */
void bm_imprime(const bitmap_t *b);              /* 8 bits por linha, como no slide 14 */
```

**Passo 1.** Implemente as funções em `a6_bitmap/bitmap.c`, usando operações de bits (`>>`, `&`, `|`), sem um `char` por unidade. A interface está em `a6_bitmap/bitmap.h`, que também declara `bm_para_lista` (Passo 3) e `bm_destroi`.

**Passo 2.** Crie uma memória de 32 unidades e marque os processos do slide 14: A (0, 5), B (8, 6), C (14, 4), D (20, 6) e E (26, 3). A saída de `bm_imprime` deve ser idêntica à do slide.

**Passo 3.** Escreva `bm_para_lista`, que percorre o mapa e imprime a lista no formato `P·0·5 -> L·5·3 -> …`. Compare com a lista do slide 14.

**Passo 4.** Meça o tempo de `bm_procura` com `clock_gettime(CLOCK_MONOTONIC, …)`. Use uma memória de 64 MiB, com 90% das unidades ocupadas em posições sorteadas, e procure k = 8, 64 e 512 com unidades de 4 B, 64 B e 4 KiB. Repita cada busca 100 vezes e registre a média. Quando não houver sequência livre do tamanho pedido, a busca percorre o mapa inteiro; anote quais buscas falharam.

**Q6.1** No Passo 3, a sua lista tem um segmento a menos que a do slide. Qual, e por quê? Que informação o SO precisa guardar fora do mapa de bits para liberar a memória de um processo?

**Q6.2** Mostre que, com unidades de *u* bytes, o mapa ocupa 1/(8u + 1) da memória, e confira o valor 1/33 do slide 15. Calcule a fração para 64 B e 4 KiB.

**Q6.3** Com unidades grandes, a última unidade de cada processo desperdiça em média metade de uma unidade. Monte uma tabela com, para cada tamanho de unidade do Passo 4, a fração ocupada pelo mapa e o desperdício médio para 100 processos. Qual tamanho você escolheria?

**Q6.4** Use as medições do Passo 4 para justificar a desvantagem citada no slide 15. Como o tempo cresce com *k* e com o tamanho do mapa?

## Atividade 7 — Lista encadeada e fusão de lacunas (slide 16)

Você vai implementar o gerenciador com lista duplamente encadeada, ordenada por endereço, que será usado também na Atividade 8. Use exatamente esta interface, que já está em `a7_a8_lista/mem.h`:

```c
#ifndef MEM_H
#define MEM_H

typedef enum { FIRST_FIT, NEXT_FIT, BEST_FIT, WORST_FIT } algoritmo_t;

typedef struct segmento {
    char tipo;                 /* 'P' = processo, 'L' = lacuna */
    int pid;                   /* -1 nas lacunas */
    unsigned inicio, tamanho;  /* em unidades de alocação */
    struct segmento *ant, *prox;
} segmento_t;

typedef struct {
    segmento_t *cabeca;        /* lista ordenada por endereço */
    segmento_t *cursor;        /* onde o next fit parou */
    unsigned total;            /* tamanho da memória */
    unsigned long visitados;   /* segmentos examinados nas buscas */
} memoria_t;

void mem_inicia(memoria_t *m, unsigned total);
int  mem_aloca(memoria_t *m, int pid, unsigned tam, algoritmo_t alg); /* início ou -1 */
int  mem_libera(memoria_t *m, int pid);                                /* 0 ou -1 */
void mem_imprime(const memoria_t *m);                                  /* P·0·5 -> L·5·3 ... */
void mem_destroi(memoria_t *m);

#endif
```

**Passo 1.** Implemente `mem.c` aceitando, por enquanto, apenas `FIRST_FIT`. Ao alocar em uma lacuna maior que o pedido, divida-a: o processo fica no início e a sobra vira uma nova lacuna logo depois dele.

**Passo 2.** Implemente `mem_libera` tratando as quatro vizinhanças do slide 16. A lacuna nova deve ser fundida com a vizinha da direita, da esquerda ou com as duas.

**Passo 3.** Teste com a memória do slide 14 (32 unidades): aloque A = 5, X1 = 3, B = 6, C = 4, X2 = 2, D = 6 e E = 3, nessa ordem, e libere X1 e X2. A saída deve ser:

```
P·0·5 -> L·5·3 -> P·8·6 -> P·14·4 -> L·18·2 -> P·20·6 -> P·26·3 -> L·29·3
```

Em seguida, libere C. Esse é o caso (b) do slide 16, e a saída deve ser:

```
P·0·5 -> L·5·3 -> P·8·6 -> L·14·6 -> P·20·6 -> P·26·3 -> L·29·3
```

**Passo 4.** Escreva um teste para cada um dos casos (a), (c) e (d), imprimindo a lista antes e depois. Rode tudo com `valgrind --leak-check=full` e corrija qualquer vazamento.

**Passo 5.** Execute a sequência da Atividade 5 (em K, com o SO como um processo de 128K no início) e confira a tabela que você preencheu à mão.

**Q7.1** Por que a lista duplamente encadeada facilita a liberação? O que mudaria na sua `mem_libera` com uma lista simplesmente encadeada?

**Q7.2** Compare a memória gasta pela lista do slide 14 com o mapa de bits da Atividade 6, supondo `sizeof(segmento_t)` na sua máquina. Em que situação a lista fica maior que o mapa?

## Atividade 8 — First, next, best e worst fit (slides 17–19)

Você vai completar `mem_aloca` com os quatro algoritmos, resolver o exercício do slide 19 com o próprio código e responder, com medições, à pergunta "quem é melhor?" do slide 17.

**Passo 1.** Implemente `NEXT_FIT`, `BEST_FIT` e `WORST_FIT`. Some 1 a `m->visitados` a cada segmento examinado em qualquer busca. Regras do *next fit*: a busca começa no segmento apontado por `m->cursor`, dá a volta ao chegar ao fim da lista e, depois de alocar, o cursor aponta para o processo recém-alocado. Se o segmento apontado pelo cursor for fundido em `mem_libera`, o cursor passa a apontar para a lacuna resultante.

**Passo 2.** Antes de rodar, resolva no papel o exercício do slide 19 e anote, para cada algoritmo, as lacunas tomadas pelos pedidos de 12, 10 e 9 KB.

**Passo 3.** Monte a memória do slide 19 no programa: com `FIRST_FIT`, aloque alternadamente cada lacuna (10, 4, 20, 18, 7, 9, 12, 15) como um processo temporário e um separador de 5 unidades. Use uma memória de exatamente 135 unidades, para não sobrar lacuna no final, e então libere os temporários. Faça `m.cursor = m.cabeca` e execute os três pedidos com cada algoritmo, cada um sobre uma cópia nova da memória. Compare com o Passo 2.

**Passo 4.** Crie `a7_a8_lista/experimento.c` com esta carga, igual para os quatro algoritmos:

| Parâmetro | Valor |
| --- | --- |
| Memória | 1024 unidades, inicialmente vazia |
| Operações | 20 000; as 2 000 primeiras são aquecimento e não entram nas médias |
| Escolha da operação | 55% alocação, 45% liberação de um processo vivo sorteado (sempre alocação se não houver processo vivo) |
| Tamanho pedido | uniforme entre 1 e 64 unidades |
| Semente | a mesma para os quatro algoritmos; repita com 5 sementes |

Meça, após cada operação: a fragmentação externa, definida como 1 − (maior lacuna ÷ total livre), e o número de lacunas. Ao final, informe também a porcentagem de pedidos que falharam e `visitados` por pedido. Apresente as médias numa tabela com um algoritmo por linha.

**Q8.1** O resultado do Passo 3 bateu com a sua resposta do Passo 2? Se não bateu, qual regra você aplicou de forma diferente?

**Q8.2** Responda a "quem é melhor?" separadamente para cada métrica do Passo 4. Existe um algoritmo que vence em todas?

**Q8.3** O slide 17 diz que o *best fit* deixa lacunas minúsculas. Conte, após cada operação fora do aquecimento, quantas lacunas de 1 ou 2 unidades existem, tire a média por algoritmo e confirme ou refute a afirmação. Por que a contagem só no fim da execução seria pouco confiável?

**Q8.4** O *next fit* examina menos segmentos que o *first fit*. Por quê? E por que a fragmentação dele tende a ser maior?

**Q8.5** O slide 18 diz que, com a lista de lacunas ordenada por tamanho, *first fit* e *best fit* são iguais. Explique por quê, e diga o que fica mais caro em `mem_libera` com essa organização.

## Desafio — Quick fit (slide 18)

O desafio vale ponto extra e mostra, na prática, as duas desvantagens que o slide 18 aponta para o *quick fit*.

**Passo 1.** Acrescente `QUICK_FIT` ao seu gerenciador, com uma lista de lacunas para cada tamanho exato 4, 8, 16, 32 e 64 unidades, mais uma lista geral para os demais tamanhos. Um pedido de tamanho exato é atendido pela lista correspondente; os outros, por *first fit* na lista geral. Ao liberar, a lacuna vai para a lista do seu tamanho, **sem fusão** com as vizinhas.

**Passo 2.** Repita o experimento da Atividade 8 com uma carga em que 80% dos pedidos têm tamanho sorteado em {4, 8, 16, 32, 64} e 20% são uniformes entre 1 e 64. Compare `QUICK_FIT` e `FIRST_FIT` em `visitados` por pedido, fragmentação externa e falhas.

**Passo 3.** Acrescente uma rotina de fusão que, a cada 100 liberações, ordena todas as lacunas por endereço, funde as adjacentes e redistribui o resultado entre as listas. Meça de novo e conte quantos segmentos essa rotina examina.

**Q.D1** Que métrica o *quick fit* melhora e qual ele piora? Use as suas medições.

**Q.D2** O que a rotina do Passo 3 custa e o que ela recupera? Qual intervalo entre fusões você escolheria para essa carga?
