# Makefile -- Laboratorio de Gerencia de Memoria (Parte 1)
#
#   make            compila base/ e solucao/
#   make base       compila apenas o codigo base (o que voce vai completar)
#   make solucao    compila apenas as solucoes de referencia
#   make clean      remove os executaveis
#
# Ao criar novos programas (por exemplo, a7_a8_lista/experimento.c), acrescente
# o executavel a lista correspondente abaixo.

CC      = gcc
CFLAGS  = -Wall -Wextra -g
OPT     = -O2

# ---------------------------------------------------------------- base --
BASE = base/a1_mapa/mapa \
       base/a2_particoes/particoes \
       base/a3_multiprog/modelo base/a3_multiprog/simula \
       base/a4_base_limite/base_limite base/a4_base_limite/protecao \
       base/a6_bitmap/teste_slide14 base/a6_bitmap/bench \
       base/a7_a8_lista/teste_slide14

# ------------------------------------------------------------- solucao --
SOL  = solucao/a3_multiprog/modelo solucao/a3_multiprog/simula \
       solucao/a4_base_limite/base_limite \
       solucao/a6_bitmap/teste_slide14 solucao/a6_bitmap/bench \
       solucao/a7_a8_lista/teste_slide14 solucao/a7_a8_lista/teste_casos \
       solucao/a7_a8_lista/troca solucao/a7_a8_lista/slide19 \
       solucao/a7_a8_lista/experimento \
       solucao/desafio_quickfit/experimento_quickfit

all: base solucao
base: $(BASE)
solucao: $(SOL)

# Programas de um unico arquivo
%/mapa: %/mapa.c
	$(CC) $(CFLAGS) $< -o $@
%/particoes: %/particoes.c
	$(CC) $(CFLAGS) $< -o $@
%/protecao: %/protecao.c
	$(CC) $(CFLAGS) $< -o $@
%/base_limite: %/base_limite.c
	$(CC) $(CFLAGS) $< -o $@

# Atividade 3: precisa de -lm
%/modelo: %/modelo.c
	$(CC) $(CFLAGS) $< -o $@ -lm
%/simula: %/simula.c
	$(CC) $(CFLAGS) $< -o $@ -lm

# Atividade 6: programas que usam bitmap.c
%/a6_bitmap/teste_slide14: %/a6_bitmap/teste_slide14.c %/a6_bitmap/bitmap.c %/a6_bitmap/bitmap.h
	$(CC) $(CFLAGS) $(filter %.c,$^) -o $@
%/a6_bitmap/bench: %/a6_bitmap/bench.c %/a6_bitmap/bitmap.c %/a6_bitmap/bitmap.h
	$(CC) $(CFLAGS) $(OPT) $(filter %.c,$^) -o $@

# Atividades 7 e 8: programas que usam mem.c
%/a7_a8_lista/teste_slide14: %/a7_a8_lista/teste_slide14.c %/a7_a8_lista/mem.c %/a7_a8_lista/mem.h
	$(CC) $(CFLAGS) $(filter %.c,$^) -o $@
%/a7_a8_lista/teste_casos: %/a7_a8_lista/teste_casos.c %/a7_a8_lista/mem.c %/a7_a8_lista/mem.h
	$(CC) $(CFLAGS) $(filter %.c,$^) -o $@
%/a7_a8_lista/troca: %/a7_a8_lista/troca.c %/a7_a8_lista/mem.c %/a7_a8_lista/mem.h
	$(CC) $(CFLAGS) $(filter %.c,$^) -o $@
%/a7_a8_lista/slide19: %/a7_a8_lista/slide19.c %/a7_a8_lista/mem.c %/a7_a8_lista/mem.h
	$(CC) $(CFLAGS) $(filter %.c,$^) -o $@
%/a7_a8_lista/experimento: %/a7_a8_lista/experimento.c %/a7_a8_lista/mem.c %/a7_a8_lista/mem.h
	$(CC) $(CFLAGS) $(OPT) $(filter %.c,$^) -o $@

# Desafio
solucao/desafio_quickfit/experimento_quickfit: solucao/desafio_quickfit/experimento_quickfit.c \
		solucao/desafio_quickfit/quickfit.c solucao/desafio_quickfit/quickfit.h \
		solucao/a7_a8_lista/mem.c solucao/a7_a8_lista/mem.h
	$(CC) $(CFLAGS) $(OPT) -Isolucao/a7_a8_lista $(filter %.c,$^) -o $@

clean:
	rm -f $(BASE) $(SOL)

.PHONY: all base solucao clean
