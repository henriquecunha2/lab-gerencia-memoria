"""grafico.py -- SOLUCAO (Atividade 3, Passo 1)

Gera o grafico do slide 8 a partir da saida de ./modelo.

Uso:  ./modelo > modelo.txt
      python3 grafico.py modelo.txt      (gera utilizacao.png)
"""
import sys
import matplotlib
matplotlib.use("Agg")
import matplotlib.pyplot as plt

arquivo = sys.argv[1] if len(sys.argv) > 1 else "modelo.txt"
with open(arquivo) as f:
    cabecalho = f.readline().split()[2:]          # ['p=0.2', 'p=0.5', 'p=0.8']
    linhas = [list(map(float, l.split())) for l in f if l.strip()]

n = [l[0] for l in linhas]
for j, rotulo in enumerate(cabecalho):
    espera = float(rotulo.split("=")[1])
    plt.plot(n, [100 * l[j + 1] for l in linhas], marker="o",
             label=f"{int(espera * 100)}% de espera por E/S")

plt.xlabel("Grau de multiprogramação (n)")
plt.ylabel("Utilização da CPU (%)")
plt.ylim(0, 100)
plt.grid(alpha=0.3)
plt.legend()
plt.savefig("utilizacao.png", dpi=150, bbox_inches="tight")
print("grafico salvo em utilizacao.png")
