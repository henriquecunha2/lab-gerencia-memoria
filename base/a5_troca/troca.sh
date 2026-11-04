#!/bin/sh
# troca.sh -- Atividade 5, Passo 2
# Mostra a memoria e a area de troca (swap) desta maquina.
# Executar:  sh troca.sh
echo "== free -h =="
free -h
echo
echo "== swapon --show =="
swapon --show || echo "(nenhuma area de troca ativa)"
