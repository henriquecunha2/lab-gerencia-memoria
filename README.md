# Laboratório: Gerência de Memória (Parte 1)

Código do roteiro *Gerência de Memória (Parte 1)* — slides 2 a 19: monoprogramação, partições fixas, grau de multiprogramação, relocação e proteção, troca de processos, mapa de bits, lista encadeada e os algoritmos *first*, *next*, *best*, *worst* e *quick fit*.

## Pré-requisitos

Linux (nativo, VM ou WSL 2) com `gcc`, `make` e `python3` (ou `gnuplot`). Para a Atividade 7, `valgrind`.

## Estrutura

```
lab-gerencia-memoria/
├── Makefile                    <- make, make base, make solucao, make clean
├── base/                       <- o que você recebe e completa
│   ├── a1_mapa/                mapa.c (fornecido)
│   ├── a2_particoes/           particoes.c (fornecido: só compilar e executar)
│   ├── a3_multiprog/           modelo.c e simula.c (TODOs)
│   ├── a4_base_limite/         base_limite.c (TODOs) e protecao.c (fornecido)
│   ├── a5_troca/               troca.sh (Passo 2)
│   ├── a6_bitmap/              bitmap.h (interface), bitmap.c e bench.c (TODOs), teste_slide14.c
│   └── a7_a8_lista/            mem.h (interface), mem.c (TODOs), teste_slide14.c
└── solucao/                    <- implementação de referência
    ├── a3_multiprog/           modelo.c, simula.c, grafico.py
    ├── a4_base_limite/         base_limite.c
    ├── a6_bitmap/              bitmap.c, teste_slide14.c, bench.c
    ├── a7_a8_lista/            mem.c, teste_slide14.c, teste_casos.c, troca.c, slide19.c, experimento.c
    └── desafio_quickfit/       quickfit.h, quickfit.c, experimento_quickfit.c
```

Nas Atividades 7 e 8 você cria novos programas em `base/a7_a8_lista/` (`teste_casos.c`, `experimento.c` etc.). Os nomes já previstos no `Makefile` são `teste_casos`, `troca`, `slide19` e `experimento`; acrescente-os à lista `BASE` quando os criar.

## Compilação

```sh
make base          # compila o código base
./base/a2_particoes/particoes
```

Todo o código compila sem avisos com `gcc -Wall -Wextra -g`.
